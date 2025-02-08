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
    html += "<h1>ESP32 Web Server</h1>";
    html += "<p>Endpoint dati: <b>/data</b></p>";
    html += "<div id='data'></div>";
    html += "<script>";
    html += "function fetchData() {";
    html += "  fetch('/data')";  //server request
    html += "    .then(response => response.json())";
    html += "    .then(data => {";
    html += "      document.getElementById('data').innerHTML = '<pre>' + JSON.stringify(data, null, 2) + '</pre>';";
    html += "    });";
    html += "}";
    html += "setInterval(fetchData, 1000);";  //update after a second
    html += "fetchData();";
    html += "</script>";
    html += "</body></html>";
    request->send(200, "text/html", html); });

  // get the most recent data
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", latestData); });

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
            { request->send(200, "application/json", latestData); });

  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", latestData); });

  server.on("/led/on", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", latestData); });

  server.on("/led/off", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", latestData); });

  server.on("/led-color", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("r")) {
        String red = request->getParam("r")->value();
    }
    if (request->hasParam("r")) {
        String green = request->getParam("g")->value();
    }
    if (request->hasParam("r")) {
        String blue = request->getParam("b")->value();
    }
    request->send(200, "application/json", latestData); });

  server.on("/led-status", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", latestData); });

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
