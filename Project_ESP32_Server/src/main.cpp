#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Definizione dei pin UART2
#define RX 16  // Modifica se necessario
#define TX 17  // Modifica se necessario

// Crea un'istanza del server
AsyncWebServer server(80);

// Parametri della rete (SSID e password dell'AP creato dall'ESP32)
const char* ssid = "ESP32-AP";
const char* password = "123456789";

// Variabile per memorizzare l'ultimo dato ricevuto
String latestData = "{}";

// Funzione per analizzare la stringa dei dati e convertirli in JSON
void processData(String data) {
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);

  if (firstComma == -1 || secondComma == -1) {
    Serial.println("Formato dati errato");
    return;
  }

  String type = data.substring(0, firstComma);
  String temperatureStr = data.substring(firstComma + 1, secondComma);
  String time = data.substring(secondComma + 1);

  float temperature = temperatureStr.toFloat();

  // Creazione dell'oggetto JSON
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["type"] = type;
  jsonDoc["temperature"] = temperature;
  jsonDoc["time"] = time;

  // Converti JSON in stringa
  latestData = "";
  serializeJson(jsonDoc, latestData);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX, TX);

  WiFi.softAP(ssid, password);
  Serial.println("Access Point avviato");
  Serial.println(WiFi.softAPIP());

  // Endpoint per la pagina principale
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<h1>ESP32 Web Server</h1><p>Endpoint dati: <b>/data</b></p>");
  });

  // Endpoint per ottenere i dati più recenti
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", latestData);
  });

  server.begin();
}

void loop() {
  if (Serial2.available()) {
    String receivedData = Serial2.readStringUntil('\n');
    receivedData.trim(); // Rimuove eventuali spazi bianchi o caratteri di nuova riga
    Serial.println("Dati ricevuti: " + receivedData);
    processData(receivedData);
  }
}
