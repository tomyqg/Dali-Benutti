// #include <Arduino.h>
// #include <ArduinoJson.h>
#include "methods.h"

const int maxClients = 5;
int clientcount = 0;

// power reset function
void powerReset() {
  // Disable interrupts
  noInterrupts();

  // Reset the ESP32 by setting the reset reason and then triggering a software reset
  //esp_reset_reason_set(ESP_RST_SW);
  esp_restart();

  // Enable interrupts again
  interrupts();
};

void webSocketEvent(const uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  StaticJsonDocument<1024> doc;
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      clientcount--;
      break;
    case WStype_CONNECTED: {
      if (clientcount < maxClients) {
        clientcount++;
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "{\"frlightcnt\":\"Connected\"}");
      } else {
        webSocket.disconnect();
      }
      break;
    }
    case WStype_TEXT:
      Serial.printf("[%u] Received Text: %s\n", num, payload);
      // parse incoming JSON message
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.println("Deserialization failed");
        Serial.println(error.c_str());
        return;
      }

      // get values from JSON message
      int lightNumber = doc["lightNumber"];
      int level = doc["level"];

      // perform action based on the incoming message
      if (doc.containsKey("findLights")) {
        //  String lights = bdali.findLights();
        doc.clear();
        // doc["lights"] = lights;
        String response;
        serializeJson(doc, response);
        webSocket.sendTXT(num, response);
      } else if (doc.containsKey("setLightLevel")) {
        bdali.setLightLevel(lightNumber, level);
      } else if (doc.containsKey("setLightOn")) {
        bdali.setLightOn(lightNumber);
      } else if (doc.containsKey("setLightOff")) {
        bdali.setLightOff(lightNumber);
      } else if (doc.containsKey("setLightUp")) {
        bdali.setLightUp(lightNumber);
      } else if (doc.containsKey("setLightDown")) {
        bdali.setLightDown(lightNumber);
      } else if (doc.containsKey("setGroupLevel")) {
        int groupNumber = doc["groupNumber"];
        bdali.setGroupLevel(groupNumber, level);
      } else if (doc.containsKey("getLightLevel")) {
        int lightLevel = bdali.getLightLevel(lightNumber);
        doc.clear();
        doc["lightLevel"] = lightLevel;
        String response;
        serializeJson(doc, response);
        webSocket.sendTXT(num, response);
      } else if (doc.containsKey("setGroupLevel")) {
        int groupNumber = doc["groupNumber"];
        int level = doc["level"];
        bdali.setGroupLevel(groupNumber, level);
        webSocket.sendTXT(num, "Group level set");
      }
       else if (doc.containsKey("reset")) {
        powerReset();
        webSocket.sendTXT(num, "Power reset performed");
      }
      else if (doc.containsKey("command") && doc["command"=="getLights"]) {

      }
    }
    return;
  }


  void getLights(){
    // 
  };

void saveLights() {
  File file = SPIFFS.open("/lights.bin", "w");
  int size = lights.size();
  file.write((uint8_t*)&size, sizeof(size));
  for (auto& instance : lights) {
    file.write((uint8_t*)&instance, sizeof(instance));
  }
  file.close();
}

void loadLights() {
  if (SPIFFS.exists("/lights.bin")) {
    File file = SPIFFS.open("/lights.bin", "r");
    int size;
    file.read((uint8_t*)&size, sizeof(size));
    lights.resize(size);
    for (auto& instance : lights) {
      file.read((uint8_t*)&instance, sizeof(instance));
    }
    file.close();
  } else {
    // code to create first instances starting by bdali.findLights()(for the shortaddresses)
    // then query all data from those shortaddresses  and finally store it in an instance.
  }
}

  void bdaliStartup(){

  };