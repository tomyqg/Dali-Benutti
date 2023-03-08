#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WebSocketsServer.h>
#include "SPIFFS.h"
#include "bendali.h"
#include <ArduinoJSON.h>
#include <ArduinoOTA.h>
#include <vector>
#include "methods.h"

// Create bdali object 
BDali bdali;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create websocket server on port 81
WebSocketsServer webSocket(81);

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";

unsigned long lastWsLVSendTime = 0;

//Variables to save values from HTML form
String ssid;
String pass;
String ip;
String gateway;

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";

IPAddress localIP;
//IPAddress localIP(192, 168, 1, 200); // hardcoded

// Set your Gateway IP address
IPAddress localGateway;
//IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

std::vector<DLight> lights;


// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }
  
  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  } 
  //file.close();
}


// Initialize WiFi
bool initWiFi() {
  if(ssid=="" || ip==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());


  if (!WiFi.config(localIP, localGateway, subnet)){
    Serial.println("STA Failed to configure");
    return false;
  }
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  return true;
}

// example to send a ardjson doc to ws
void sendJSON() {
  // Populate the ArduinoJSON object with the specified values
  StaticJsonDocument<500> jsonDoc;
  jsonDoc.clear();
  jsonDoc["command"] = "levels";

  JsonArray friendlyLights = jsonDoc.createNestedArray("lights");
  JsonObject light1 = friendlyLights.createNestedObject();
  light1["shortaddress"] = 0;
  light1["level"] = 254;

  JsonObject light2 = friendlyLights.createNestedObject();
  light2["shortaddress"] = 1;
  light2["level"] = 150;

  JsonObject light3 = friendlyLights.createNestedObject();
  light3["shortaddress"] = 2;
  light3["level"] = 0;

  // Convert the ArduinoJSON object to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Send the JSON string over the websocket
  webSocket.broadcastTXT(jsonString);
}






void setup() {
  // Start BDali
  bdali.begin(SDA_PIN,SCL_PIN);

  Serial.begin(115200);

  initSPIFFS();

  // Load values saved in SPIFFS
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);
  ip = readFile(SPIFFS, ipPath);
  gateway = readFile (SPIFFS, gatewayPath);
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(gateway);


  if(initWiFi()) {
    // Route for root / web page
    Serial.println("webserver initiated");
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/index.html", "text/html");
    });
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
    // loading of the dlight instances
    loadLights();
      // Websockets setup
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
     // Initialize OTA
    ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
          type = "sketch";
        } else {
          type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
        ESP.restart();
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
          Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
          Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
          Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
          Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
          Serial.println("End Failed");
        }
      });

    ArduinoOTA.begin();

  }
  else {
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("BDALI", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/wifimanager.html", "text/html");
    });
    
    server.serveStatic("/", SPIFFS, "/");
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(SPIFFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
           
            Serial.println(ip);
            // Write file to save value
            writeFile(SPIFFS, ipPath, ip.c_str());
          }
          // HTTP POST gateway value
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
            Serial.print("Gateway set to: ");
            Serial.println(gateway);
            // Write file to save value
            writeFile(SPIFFS, gatewayPath, gateway.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
      delay(3000);
      ESP.restart();
    });

    server.begin();


  }
}

void loop() {
  webSocket.loop();
  ArduinoOTA.handle();
  //   if (millis() - lastWsLVSendTime >= interval) {
  //   // Send WebSocket message
  //   sendJSON();
  //   lastWsLVSendTime = millis(); // Update last send time
  // }

  //   // Find the list of groups that at least one light is a member of
  //   std::vector<uint8_t> groupNumbers = bdali.findGroups();

  //   // Print the list of groups to the serial monitor
  //   Serial.print("Groups found: ");
  //   for (int i = 0; i < groupNumbers.size(); i++) {
  //       Serial.print(groupNumbers[i]);
  //       Serial.print(" ");
  //   }
  //   Serial.println();




    // // Discover the short addresses of all the DALI lights on the bus
    // std::vector<uint8_t> shortAddresses = bdali.findLights();

    // // Print the short addresses to the serial monitor
    // Serial.print("Found ");
    // Serial.print(shortAddresses.size());
    // Serial.println(" DALI lights on the bus:");
    // for (int i = 0; i < shortAddresses.size(); i++) {
    //     Serial.print("  - Short address ");
    //     Serial.println(shortAddresses[i]);
    // }
// uint8_t found = bdali.commission(DA_YES);
// Serial.print(found);



    //   // Get the scene level information for light 1
    // uint8_t* sceneLevels = bdali.getSceneLevels(14);

    // // Print the scene level information for each scene
    // for (int i = 0; i < 16; i++) {
    //     Serial.print("Scene ");
    //     Serial.print(i);
    //     Serial.print(": ");
    //     Serial.println(sceneLevels[i]);
    // }

    // // Free the memory allocated for the sceneLevels array
    // delete[] sceneLevels;

    // Wait for some time before repeating
    // delay(50000);



//  if (Serial.available() > 0) {
//     String input = Serial.readStringUntil('\n');
//     String method = input.substring(0, input.indexOf('('));
//     int param1 = input.substring(input.indexOf('(') + 1, input.indexOf(',')).toInt();
//     int param2 = input.substring(input.indexOf(',') + 1, input.indexOf(')')).toInt();

//     if (method == "SetLight") {
//       bdali.setLightLevel(param1,param2);
//     } 
//     else if (method == "GetLight") {
//       Serial.println(bdali.getLightLevel(param1));
//     } 
//     else if (method == "SetGroup") {
//       bdali.setGroupLevel(param1,param2);
//     }  
//     else if (method == "GetGroup") {
//       Serial.println(bdali.getGroupLevel(param1));
//     } 
//     else if (method == "FindLights") {
//       Serial.println(bdali.findLights());
//     } 
//     else if (method == "LightOn") {
//       // only dali2
//       bdali.setLightOn(param1);
//     }
//     else if (method == "LightOff") {
//       bdali.setLightOff(param1);
//     }
//     else if (method == "LightUp") {
//       bdali.setLightUp(param1);
//     }
//     else if (method == "LightDown") {
//       bdali.setLightDown(param1);
//     }
//     else {
//       Serial.println("Wrong input");
//       // handle invalid input
//     }
//   }

}
