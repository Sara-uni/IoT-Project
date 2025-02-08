#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// UART pins on the ESP32
#define RX 16
#define TX 17

AsyncWebServer server(80);

// parameters for a existing wifi network (required to get the timestamp)
const char *ssidWifi = "WRL#12IRIDEOS";
const char *pswWifi = "Matty!!2003";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

// parameters for the network created by the ESP32
const char *ssid = "ESP32-AP";
const char *password = "123456789";

// variable storing the last received data
String latestData = "{}";

// get the current time and date using wifi
String getFormattedTimestamp()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("TIMESTAMP Error");
    return "0000-00-00T00:00:00Z";
  }

  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo); // ISO 8601 format
  return String(buffer);
}

// give the right JSON format to the data we acquired
void processData(String data)
{
  int firstComma = data.indexOf(',');

  if (firstComma == -1)
  {
    Serial.println("Wrong data format");
    return;
  }

  String type = data.substring(0, firstComma);
  String value = data.substring(firstComma + 1);

  StaticJsonDocument<200> jsonDoc;
  jsonDoc["type"] = type;
  jsonDoc["value"] = value;
  jsonDoc["time"] = getFormattedTimestamp();

  latestData = "";
  serializeJson(jsonDoc, latestData);
}

void processLed(String data)
{
  int firstComma = data.indexOf(',');

  if (firstComma == -1)
  {
    Serial.println("Wrong data format");
    return;
  }

  String active = data.substring(0, firstComma);
  String colors = data.substring(firstComma + 1);

  StaticJsonDocument<200> jsonDoc;
  jsonDoc["active"] = active;

  if (active == "true")
  {
    int redIndex = colors.indexOf(',');
    String red = colors.substring(0, redIndex);
    colors = colors.substring(redIndex + 1);
    jsonDoc["r"] = red;

    int greenIndex = colors.indexOf(',');
    String green = colors.substring(0, greenIndex);
    colors = colors.substring(greenIndex + 1);
    jsonDoc["g"] = green;

    int blueIndex = colors.indexOf(',');
    String blue = colors.substring(0, blueIndex);
    colors = colors.substring(blueIndex + 1);
    jsonDoc["b"] = blue;
  }

  latestData = "";
  serializeJson(jsonDoc, latestData);
}

void tempSetup()
{
  // existing wifi configuration
  WiFi.begin(ssidWifi, pswWifi);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // access point configuration
  WiFi.softAP(ssid, password);
  Serial.println("Access Point ready");
  Serial.println(WiFi.softAPIP());

  // javascript to update the data every second
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String html = "<html><body>";
    html += "<h1>ESP32 Web Server Attivo</h1>";
    html += "</body></html>";
    request->send(200, "text/html", html); });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
            {  
              Serial2.println("GET_TEMP");
              unsigned long timeout = millis() + 1000; // Timeout di 1 secondo
              while (!Serial2.available()) {
                  if (millis() > timeout) {
                      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
                      return;
                  }
              }
              String receivedData = Serial2.readStringUntil('\n');
              processData(receivedData);
              request->send(200, "application/json", latestData); });

  server.on("/noise", HTTP_GET, [](AsyncWebServerRequest *request)
            { Serial2.println("GET_NOISE");
              unsigned long timeout = millis() + 1000; // Timeout di 1 secondo
              while (!Serial2.available()) {
                  if (millis() > timeout) {
                      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
                      return;
                  }
              }
              String receivedData = Serial2.readStringUntil('\n');
              processData(receivedData);
              request->send(200, "application/json", latestData); });

  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request)
            { Serial2.println("GET_LIGHT");
              unsigned long timeout = millis() + 1000; // Timeout di 1 secondo
              while (!Serial2.available()) {
                  if (millis() > timeout) {
                      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
                      return;
                  }
              }
              String receivedData = Serial2.readStringUntil('\n');
              processData(receivedData);
              request->send(200, "application/json", latestData); });

  server.on("/led/on", HTTP_GET, [](AsyncWebServerRequest *request)
            { Serial2.println("LED_ON");
              unsigned long timeout = millis() + 1000; // Timeout di 1 secondo
              while (!Serial2.available()) {
                  if (millis() > timeout) {
                      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
                      return;
                  }
              }
              request->send(200, "application/json", "{\"success\":\"The led is on!\"}"); });

  server.on("/led/off", HTTP_GET, [](AsyncWebServerRequest *request)
            { Serial2.println("LED_OFF");
              unsigned long timeout = millis() + 1000; // Timeout di 1 secondo
              while (!Serial2.available()) {
                  if (millis() > timeout) {
                      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
                      return;
                  }
              }
              request->send(200, "application/json", "{\"success\":\"The led is off!\"}"); });

  server.on("/led-color", HTTP_GET, [](AsyncWebServerRequest *request)
            {
                String red = "0";
                String green = "0";
                String blue = "0";
            
                if (request->hasParam("r")) {
                  red = request->getParam("r")->value();
                  if (red.toInt() < 0 || red.toInt() > 255) {
                    request->send(400, "application/json", "{\"error\":\"Invalid red value\"}");
                    return;
                  }
                }
              
                if (request->hasParam("g")) {
                  green = request->getParam("g")->value();
                  if (green.toInt() < 0 || green.toInt() > 255) {
                    request->send(400, "application/json", "{\"error\":\"Invalid green value\"}");
                    return;
                  }
                }
                  
                if (request->hasParam("b")) {
                    blue = request->getParam("b")->value();
                    if (blue.toInt() < 0 || blue.toInt() > 255) {
                        request->send(400, "application/json", "{\"error\":\"Invalid blue value\"}");
                        return;
                    }
                }
                
                char command[50];
                snprintf(command, sizeof(command), "SET_COLOR(%s, %s, %s)", red.c_str(), green.c_str(), blue.c_str());
                Serial2.println(command);
                unsigned long timeout = millis() + 1000; // Timeout di 1 secondo
                while (!Serial2.available()) {
                    if (millis() > timeout) {
                        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
                        return;
                    }
                }
                request->send(200, "application/json", "{\"success\":\"Changed color!\"}"); });

  server.on("/led-status", HTTP_GET, [](AsyncWebServerRequest *request)
            { Serial2.println("GET_LED");
              unsigned long timeout = millis() + 1000; // Timeout di 1 secondo
              while (!Serial2.available()) {
                  if (millis() > timeout) {
                      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
                      return;
                  }
              }
              String receivedData = Serial2.readStringUntil('\n');
              processLed(receivedData);
              request->send(200, "application/json", latestData); });

  server.begin();
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX, TX);
  tempSetup();
}

void loop()
{
  if (Serial2.available())
  {

    String receivedData = Serial2.readStringUntil('\n');

    receivedData.trim();
    Serial.println("received: " + receivedData); // you can check the arriving data by monitoring the port
    processData(receivedData);
  }
}
