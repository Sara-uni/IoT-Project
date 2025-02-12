#ifndef API_HANDLER_H
#define API_HANDLER_H

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void temperatureHandler(AsyncWebServerRequest *request, bool vocalRequest = false);
void noiseHandler(AsyncWebServerRequest *request, bool vocalRequest = false);
void lightHandler(AsyncWebServerRequest *request, bool vocalRequest = false);
void ledOnHandler(AsyncWebServerRequest *request);
void ledOffHandler(AsyncWebServerRequest *request);
void setColorHandler(AsyncWebServerRequest *request);
void getLedStatusHandler(AsyncWebServerRequest *request);
void voiceCommandHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

#endif
