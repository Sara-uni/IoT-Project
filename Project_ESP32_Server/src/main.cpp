#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "api_handler.h"

// UART pins on the ESP32
#define RX 16
#define TX 17

AsyncWebServer server(80);

// parameters for a existing wifi network (required to get the timestamp)
const char *ssidWifi = "OnePlus Nord";
const char *pswWifi = "androids";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

// parameters for the network created by the ESP32
const char *ssid = "ESP32-AP";
const char *password = "123456789";

void serverSetup()
{
  Serial.println("Connecting to WiFi...");
  Serial2.println("PRINT(Connecting to WiFi, Please wait)");
  WiFi.begin(ssidWifi, pswWifi);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    unsigned long timeout = millis() + 500;
    while (millis() < timeout)
    {
      yield();
    }

    Serial.print(".");
    attempts++;
    if (attempts > 30)
    { // Timeout after 15 seconds
      Serial.println("\nFailed to connect to WiFi! Restarting...");
      Serial2.println("PRINT(Failed to connect, Restarting...)");
      ESP.restart();
    }
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  String ipMessage = "PRINT(IP Address:, " + WiFi.localIP().toString() + ")";
  Serial2.println(ipMessage);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Access Point Configuration
  Serial.println("Starting Access Point...");
  bool apStarted = WiFi.softAP(ssid, password);
  if (apStarted)
  {
    Serial.println("AP Started! IP Address: ");
    Serial.println(WiFi.softAPIP());
  }
  else
  {
    Serial.println("Failed to start AP!");
  }

  // API
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String html = "<html><body>";
    html += "<h1>ESP32 Web Server Attivo</h1>";
    html += "</body></html>";
    request->send(200, "text/html", html); });

  server.on("/temperature", HTTP_GET, temperatureHandler);
  server.on("/noise", HTTP_GET, noiseHandler);
  server.on("/light", HTTP_GET, lightHandler);
  server.on("/led/on", HTTP_GET, ledOnHandler);
  server.on("/led/off", HTTP_GET, ledOffHandler);
  server.on("/led-color", HTTP_GET, setColorHandler);
  server.on("/led-status", HTTP_GET, getLedStatusHandler);
  server.on("/command", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, voiceCommandHandler);

  // Start server
  Serial.println("Starting Web Server...");
  server.begin();
  Serial.println("Web Server started!");
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX, TX);
  serverSetup();
}
void loop()
{
  yield();
  static unsigned long lastCheckTime = 0;

  if (millis() - lastCheckTime > 5000)
  {
    lastCheckTime = millis();

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi DISCONNECTED! Trying to reconnect...");
      Serial2.println("PRINT(WiFi DISCONNECTED!, Trying to reconnect)");

      WiFi.disconnect();
      WiFi.reconnect();

      unsigned long reconnectTimeout = millis() + 50000;
      while (WiFi.status() != WL_CONNECTED && millis() < reconnectTimeout)
      {
        unsigned long timeout = millis() + 500;
        while (millis() < timeout)
        {
          yield();
        }
        Serial.print(".");
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("\nWiFi RECONNECTED!");
        Serial.print("New IP: ");
        Serial.println(WiFi.localIP());

        char ipMessage[60];
        sprintf(ipMessage, "PRINT(%s, %s)", "IP Address:", WiFi.localIP());
        Serial2.println(ipMessage);
      }
      else
      {
        Serial.println("Failed to reconnect!, Retry..");
      }
    }
  }
}
