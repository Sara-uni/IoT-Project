#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>

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

  JsonDocument jsonDoc;
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

  JsonDocument jsonDoc;
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

bool waitResponse(int timeoutMs = 1000)
{
  unsigned long timeout = millis() + timeoutMs;
  while (!Serial2.available())
  {
    if (millis() > timeout)
    {
      return false;
    }
    yield();
  }
  return true;
}

void serverSetup()
{
  Serial.println("Connecting to WiFi...");
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
    { // Timeout dopo 15 secondi
      Serial.println("\nFailed to connect to WiFi! Restarting...");
      ESP.restart();
    }
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Configurazione Access Point
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

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial2.println("GET_TEMP");
    Serial.println("Sent command: GET_TEMP to MSP432");

    bool getResponse = waitResponse();
    if(!getResponse) {
      Serial.println("Error: Timeout waiting for response from MSP432.");
      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
      return;
    }
    
    String receivedData = Serial2.readStringUntil('\n');
    processData(receivedData);
    request->send(200, "application/json", latestData); });

  server.on("/noise", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
    Serial2.println("GET_NOISE");
    Serial.println("Sent command: GET_NOISE to MSP432");

    bool getResponse = waitResponse();
    if(!getResponse) {
      Serial.println("Error: Timeout waiting for response from MSP432.");
      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
      return;
    }
    
    String receivedData = Serial2.readStringUntil('\n');
    processData(receivedData);
    request->send(200, "application/json", latestData); });
  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
    Serial2.println("GET_LIGHT");
    Serial.println("Sent command: GET_LIGHT to MSP432");
    
    bool getResponse = waitResponse();
    if(!getResponse) {
      Serial.println("Error: Timeout waiting for response from MSP432.");
      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
      return;
    }
    
    String receivedData = Serial2.readStringUntil('\n');
    processData(receivedData);
    Serial.print("Received from MSP432: ");
    Serial.println(receivedData);
    request->send(200, "application/json", latestData); });

  server.on("/led/on", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              Serial2.println("LED_ON");
              Serial.println("Sent command: LED_ON to MSP432");

              bool getResponse = waitResponse();
              if(!getResponse) {
                Serial.println("Error: Timeout waiting for response from MSP432.");
                request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
                return;
              }
              
              request->send(200, "application/json", "{\"success\":\"The led is on!\"}"); });

  server.on("/led/off", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              Serial2.println("LED_OFF");
              Serial.println("Sent command: LED_OFF to MSP432");

              bool getResponse = waitResponse();
              if(!getResponse) {
                Serial.println("Error: Timeout waiting for response from MSP432.");
                request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
                return;
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

    char command[30];
    snprintf(command, sizeof(command), "SET_COLOR(%s, %s, %s)", red.c_str(), green.c_str(), blue.c_str());
    Serial2.println(command);

    char string[256];
    sprintf(string, "Sent command: %s", command);
    Serial.println(command);
                  
    bool getResponse = waitResponse();
    if(!getResponse) {
      Serial.println("Error: Timeout waiting for response from MSP432.");
      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
      return;
    }
    
    request->send(200, "application/json", "{\"success\":\"Changed color!\"}"); });

  server.on("/led-status", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial2.println("GET_LED");
    Serial.println("Sent command: GET_LED to MSP432");
     
    bool getResponse = waitResponse();
    if(!getResponse) {
      Serial.println("Error: Timeout waiting for response from MSP432.");
      request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
      return;
    }
    
    String receivedData = Serial2.readStringUntil('\n');
    processLed(receivedData);
    request->send(200, "application/json", latestData); });

  // Avvio del server
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
  esp_task_wdt_reset();
  yield();
  static unsigned long lastCheckTime = 0;
  // Controllo WiFi ogni 5 secondi
  if (millis() - lastCheckTime > 5000)
  {
    lastCheckTime = millis();

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi DISCONNECTED! Trying to reconnect...");
      WiFi.disconnect();
      WiFi.reconnect();

      unsigned long reconnectTimeout = millis() + 50000; // Timeout di 10 secondi
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
      }
      else
      {
        Serial.println("\nFailed to reconnect.");
      }
    }
  }
}
