#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "api_handler.h"

// ip server rasa
const char *rasaIP = "192.168.4.2";

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
String processData(String data)
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

    String formattedData = "";
    serializeJson(jsonDoc, formattedData);
    return formattedData;
}

String processLed(String data)
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

    String formattedData = "";
    serializeJson(jsonDoc, formattedData);
    return formattedData;
}

void temperatureHandler(AsyncWebServerRequest *request)
{
    Serial2.println("GET_TEMP");
    Serial.println("Sent command: GET_TEMP to MSP432");

    bool getResponse = waitResponse();
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = Serial2.readStringUntil('\n');
    request->send(200, "application/json", processData(receivedData));
}

void noiseHandler(AsyncWebServerRequest *request)
{
    Serial2.println("GET_NOISE");
    Serial.println("Sent command: GET_NOISE to MSP432");

    bool getResponse = waitResponse();
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = Serial2.readStringUntil('\n');
    request->send(200, "application/json", processData(receivedData));
}

void lightHandler(AsyncWebServerRequest *request)
{
    Serial2.println("GET_LIGHT");
    Serial.println("Sent command: GET_LIGHT to MSP432");

    bool getResponse = waitResponse();
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = Serial2.readStringUntil('\n');
    request->send(200, "application/json", processData(receivedData));
}

void ledOnHandler(AsyncWebServerRequest *request)
{
    Serial2.println("LED_ON");
    Serial.println("Sent command: LED_ON to MSP432");

    bool getResponse = waitResponse();
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    request->send(200, "application/json", "{\"success\":\"The led is on!\"}");
}

void ledOffHandler(AsyncWebServerRequest *request)
{
    Serial2.println("LED_OFF");
    Serial.println("Sent command: LED_OFF to MSP432");

    bool getResponse = waitResponse();
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    request->send(200, "application/json", "{\"success\":\"The led is off!\"}");
}

void setColorHandler(AsyncWebServerRequest *request)
{
    String red = "0";
    String green = "0";
    String blue = "0";

    if (request->hasParam("r"))
    {
        red = request->getParam("r")->value();
        if (red.toInt() < 0 || red.toInt() > 255)
        {
            request->send(400, "application/json", "{\"error\":\"Invalid red value\"}");
            return;
        }
    }

    if (request->hasParam("g"))
    {
        green = request->getParam("g")->value();
        if (green.toInt() < 0 || green.toInt() > 255)
        {
            request->send(400, "application/json", "{\"error\":\"Invalid green value\"}");
            return;
        }
    }

    if (request->hasParam("b"))
    {
        blue = request->getParam("b")->value();
        if (blue.toInt() < 0 || blue.toInt() > 255)
        {
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
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    request->send(200, "application/json", "{\"success\":\"Changed color!\"}");
}

void getLedStatusHandler(AsyncWebServerRequest *request)
{
    Serial2.println("GET_LED");
    Serial.println("Sent command: GET_LED to MSP432");

    bool getResponse = waitResponse();
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = Serial2.readStringUntil('\n');
    request->send(200, "application/json", processLed(receivedData));
}

void voiceCommandHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String receivedPayload = String((char *)data).substring(0, len);
    Serial.println("Received JSON: " + receivedPayload);

    // Invia il payload all'MSP432 tramite UART
    Serial2.println(receivedPayload);

    // send command to rasa server
    HTTPClient http;
    http.begin("http://" + String(rasaIP) + ":5005/webhooks/rest/webhook");
    http.addHeader("Content-Type", "application/json");

    // char jsonPayload[256];
    // sprintf(jsonPayload, "{\"sender\": \"esp32\", \"message\": %s }", receivedPayload);
    int httpResponseCode = http.POST(receivedPayload);

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        Serial.println("Rasa response: " + response);
    }
    else
    {
        Serial.println("Error in HTTP request");
    }

    http.end();

    Serial.println("Sent command: " + receivedPayload);

    // Attendi una conferma dalla UART
    bool responseOk = waitResponse();
    if (!responseOk)
    {
        Serial.println("Error: timeout by MSP432 response.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    // Ricevi la risposta dall'MSP432
    String mspResponse = Serial2.readStringUntil('\n');
    Serial.println("MSP432 response: " + mspResponse);

    request->send(200, "application/json", "{\"status\":\"success\",\"response\":\"" + mspResponse + "\"}");
}