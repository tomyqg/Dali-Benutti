#ifndef METHODS_H
#define METHODS_H
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "bendali.h"
#include <SPIFFS.h>
extern WebSocketsServer webSocket;
extern BDali* bdali;
extern std::vector<DLight> lights;
void sendDLights(uint8_t num);
void eraseLights();
void powerReset();
void sendLevels();
void saveLights();
void loadLights();
void webSocketEvent(const uint8_t num, WStype_t type, uint8_t * payload, size_t length);



#endif // METHODS_H