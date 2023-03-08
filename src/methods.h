#ifndef METHODS_H
#define METHODS_H
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "bendali.h"
#include <SPIFFS.h>
extern WebSocketsServer webSocket;
extern BDali bdali;
extern std::vector<DLight> lights;
void webSocketEvent(const uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void loadLights();
void saveLights();
void getLights();
#endif // METHODS_H