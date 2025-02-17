#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "api_handler.h"
#include <vector>

// ip server rasa
const char *rasaIp = "192.168.4.2";
std::vector<String> responseQueue;

bool waitResponse(String expectedType, int timeoutMs = 1000)
{
    unsigned long timeout = millis() + timeoutMs;
    while (millis() < timeout)
    {
        if (Serial2.available())
        {
            String receivedData = Serial2.readStringUntil('\n');
            responseQueue.push_back(receivedData);

            if (receivedData.startsWith(expectedType))
            {
                return true;
            }
        }
    }
    return false;
}

String getResponseFromQueue(String expectedType)
{
    for (size_t i = 0; i < responseQueue.size(); i++)
    {
        if (responseQueue[i].startsWith(expectedType))
        {
            String response = responseQueue[i];
            responseQueue.erase(responseQueue.begin() + i);
            return response;
        }
    }
    return "";
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
    Serial.print("Data to process: ");
    Serial.println(data);
    int firstComma = data.indexOf(',');
    if (firstComma == -1)
    {
        Serial.println("Wrong data format");
        JsonDocument jsonDoc;
        jsonDoc["error"] = "Wrong data format";

        String formattedData = "";
        serializeJson(jsonDoc, formattedData);
        return formattedData;
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
    Serial.print("Led Data to process: ");
    Serial.println(data);
    int firstComma = data.indexOf(',');
    if (firstComma == -1)
    {
        Serial.println("Wrong data format");
        JsonDocument jsonDoc;
        jsonDoc["error"] = "Wrong data format";

        String formattedData = "";
        serializeJson(jsonDoc, formattedData);
        return formattedData;
    }

    String type = data.substring(0, firstComma);
    String value = data.substring(firstComma + 1);

    if (value == "true" || value == "false")
    {
        JsonDocument jsonDoc;
        jsonDoc["active"] = value;

        String formattedData = "";
        serializeJson(jsonDoc, formattedData);
        return formattedData;
    }

    JsonDocument jsonDoc;
    jsonDoc["error"] = "Wrong data format";

    String formattedData = "";
    serializeJson(jsonDoc, formattedData);
    return formattedData;
}

void temperatureHandler(AsyncWebServerRequest *request)
{
    Serial2.println("GET_TEMP");
    Serial.println("\nSent command: GET_TEMP to MSP432");

    bool getResponse = waitResponse("temperature");
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = getResponseFromQueue("temperature");
    request->send(200, "application/json", processData(receivedData));
}

void noiseHandler(AsyncWebServerRequest *request)
{
    Serial2.println("GET_NOISE");
    Serial.println("\nSent command: GET_NOISE to MSP432");

    bool getResponse = waitResponse("noise");
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = getResponseFromQueue("noise");
    request->send(200, "application/json", processData(receivedData));
}

void lightHandler(AsyncWebServerRequest *request)
{
    Serial2.println("GET_LIGHT");
    Serial.println("\nSent command: GET_LIGHT to MSP432");

    bool getResponse = waitResponse("light");
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = getResponseFromQueue("light");
    request->send(200, "application/json", processData(receivedData));
}

void ledOnHandler(AsyncWebServerRequest *request)
{
    Serial2.println("LED_ON");
    Serial.println("\nSent command: LED_ON to MSP432");

    bool getResponse = waitResponse("led");
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = getResponseFromQueue("led");
    request->send(200, "application/json", "{\"executed\":\"LED_ON\"}");
}

void ledOffHandler(AsyncWebServerRequest *request)
{
    Serial2.println("LED_OFF");
    Serial.println("\nSent command: LED_OFF to MSP432");

    bool getResponse = waitResponse("led");
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = getResponseFromQueue("led");
    request->send(200, "application/json", "{\"executed\":\"LED_OFF\"}");
}

void setColorInternal(AsyncWebServerRequest *request, String command)
{
    Serial2.println(command);

    char string[256];
    sprintf(string, "\nSent command: %s", command);
    Serial.println(command);

    bool getResponse = waitResponse("setColor");
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = getResponseFromQueue("setColor");
    char commandExecuted[256];
    sprintf(commandExecuted, "{\"executed\":\"%s\"}", command);
    request->send(200, "application/json", "{\"executed\":\"LED_ON\"}");
    //request->send(200, "application/json", commandExecuted);
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

    setColorInternal(request, command);
}

void getLedStatusHandler(AsyncWebServerRequest *request)
{
    Serial2.println("GET_LED");
    Serial.println("\nSent command: GET_LED to MSP432");

    bool getResponse = waitResponse("ledStatus");
    if (!getResponse)
    {
        Serial.println("Error: Timeout waiting for response from MSP432.");
        request->send(500, "application/json", "{\"error\":\"Timeout MSP432\"}");
        return;
    }

    String receivedData = getResponseFromQueue("ledStatus");
    request->send(200, "application/json", processLed(receivedData));
}

String getRasaResponse(String rawResponse)
{
    Serial.print("raw: ");
    Serial.println(rawResponse);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, rawResponse);

    if (error)
    {
        Serial.print("Errore nella deserializzazione del JSON: ");
        Serial.println(error.f_str());
        return "";
    }

    return String(doc[0]["text"].as<const char *>());
}

void voiceCommandHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String receivedPayload = String((char *)data).substring(0, len);
    Serial.println("Received JSON: " + receivedPayload);

    // send command to rasa server
    HTTPClient http;
    http.begin("http://" + String(rasaIp) + ":5005/webhooks/rest/webhook");
    http.addHeader("Content-Type", "application/json");

    char jsonPayload[516];
    sprintf(jsonPayload, "{\"sender\": \"esp32\", \"message\": \"%s\" }", receivedPayload.c_str());
    Serial.println(jsonPayload);
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        String command = getRasaResponse(response);
        Serial.println(command);
        if (command == "GET_TEMP")
        {
            temperatureHandler(request);
        }
        else if (command == "GET_LIGHT")
        {
            lightHandler(request);
        }
        else if (command == "GET_NOISE")
        {
            noiseHandler(request);
        }
        else if (command == "LED_ON")
        {
            ledOnHandler(request);
        }
        else if (command == "LED_OFF")
        {
            ledOffHandler(request);
        }
        else if (command.startsWith("SET_COLOR"))
        {
            setColorInternal(request, command);
        }
        else
        {
            request->send(400, "application/json", "{\"error\":\"Command could not be interpreted.\"}");
        }
    }
    else
    {
        Serial.println("Error in HTTP request");
        request->send(400, "application/json", "{\"error\":\"Error in HTTP request\"}");
    }

    http.end();
}