  #include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SoftwareSerial.h>

#define RX_PIN D7  // Define RX pin for SoftwareSerial
#define TX_PIN D8  // Define TX pin for SoftwareSerial

SoftwareSerial espSerial(RX_PIN, TX_PIN);  // Create SoftwareSerial object for UART communication

const char* wifi_ssid = "Wis";
const char* wifi_password = "Quy*2002";

AsyncWebServer server(80);

bool wifiConnected = false;

uint8_t buffer[256];  // Buffer for UART data
int dataIndex = 0;  // Index for the buffer

volatile float voltage = 0.0;
volatile float current = 0.0;
volatile float temperature = 0.0;
volatile float power = 0.0;

void setup() {
  Serial.begin(9600);  // Hardware serial for debugging

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount SPIFFS");
    return;
  } else {
    Serial.println("SPIFFS mounted successfully");
  }

  checkSPIFFSFiles();
  connectToWiFi();
  setupServer();
  server.begin();
}

void loop() {
  if (wifiConnected) {
    handleUART();
  }
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    connectToWiFi();
  }
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  wifiConnected = true;
  espSerial.begin(9600);
}

void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index1.html", "text/html");
  });

  // Serve additional HTML files
  server.on("/index1.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index1.html", "text/html");
  });
  server.on("/index2.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index2.html", "text/html");
  });
  server.on("/index3.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index3.html", "text/html");
  });

  // Serve CSS files
  server.on("/css/main.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/main.css", "text/css");
  });
  server.on("/css/base.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/base.css", "text/css");
  });
  server.on("/css/responsive.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/responsive.css", "text/css");
  });

  // Serve JavaScript files
  server.on("/java/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/java/script.js", "text/javascript");
  });

  // Serve sensor data
  server.on("/voltage", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(voltage, 2).c_str());
  });
  server.on("/current", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(current, 2).c_str());
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(temperature, 2).c_str());
  });
  server.on("/power", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(power, 2).c_str());
  });

  // Command handling routes
  server.on("/start", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("data", true)) {
      String data = request->getParam("data", true)->value();
      data.replace(" ", "\n");
      String formattedData = formatCoordinates(data);
      handleCommand(request, formattedData);
    } else {
      handleCommand(request, "START");
    }
  });
  server.on("/stop", HTTP_POST, [](AsyncWebServerRequest *request){
    handleCommand(request, "STOP");
  });
  server.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request){
    handleCommand(request, "RESET");
  });
  server.on("/low", HTTP_POST, [](AsyncWebServerRequest *request){
    handleCommand(request, "LOW");
  });
  server.on("/medium", HTTP_POST, [](AsyncWebServerRequest *request){
    handleCommand(request, "MEDIUM");
  });
  server.on("/high", HTTP_POST, [](AsyncWebServerRequest *request){
    handleCommand(request, "HIGH");
  });
  server.on("/drill/on", HTTP_POST, [](AsyncWebServerRequest *request){
    handleCommand(request, "ON");
  });
  server.on("/drill/off", HTTP_POST, [](AsyncWebServerRequest *request){
    handleCommand(request, "OFF");
  });
  server.on("/xyz", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("data", true)) {
      String data = request->getParam("data", true)->value();
      handleCommand(request, "xyz," + data);
    } else {
      request->send(400, "text/plain", "Missing data parameter");
    }
  });
}

String formatCoordinates(const String& data) {
  String formattedData;
  int start = 0;
  int end = data.indexOf('\n');

  while (end != -1) {
    formattedData += "GOTO" + data.substring(start, end) + "\n";
    start = end + 1;
    end = data.indexOf('\n', start);
  }

  formattedData += "GOTO" + data.substring(start) + "\n";

  return formattedData;
}

void handleCommand(AsyncWebServerRequest *request, const String& command) {
  Serial.println(command);
  espSerial.println(command);
  request->send(200, "text/plain", "Command sent: " + command);
}

void processReceivedData(char* data) {
  if (strcmp(data, "IP") == 0) {
    espSerial.println(WiFi.localIP().toString());
  } else {
    char* token = strtok(data, ",");
    while (token != NULL) {
      if (strncmp(token, "V:", 2) == 0) {
        voltage = atof(token + 2);
      } else if (strncmp(token, "C:", 2) == 0) {
        current = atof(token + 2);
      } else if (strncmp(token, "T:", 2) == 0) {
        temperature = atof(token + 2);
      } else if (strncmp(token, "P:", 2) == 0) {
        power = atof(token + 2);
      }
      token = strtok(NULL, ",");
    }

    Serial.print("Voltage: ");
    Serial.println(voltage);
    Serial.print("Current: ");
    Serial.println(current);
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Power: ");
    Serial.println(power);
  }
}

void handleUART() {
  while (espSerial.available()) {
    char c = espSerial.read();
    if (isPrintable(c) || c == '\n' || c == '\r') {
      if (c == '\n' || c == '\r') {
        if (dataIndex > 0) {
          buffer[dataIndex] = '\0';
          Serial.print("Received data: ");
          Serial.println((char*)buffer);
          processReceivedData((char*)buffer);
          dataIndex = 0;
        }
      } else {
        if (dataIndex < sizeof(buffer) - 1) {
          buffer[dataIndex++] = c;
        } else {
          Serial.println("Buffer overflow, data reset");
          dataIndex = 0;
        }
      }
    }
  }
}

void checkSPIFFSFiles() {
  const char* files[] = {
    "/index1.html",
    "/index2.html",
    "/index3.html",
    "/css/main.css",
    "/css/base.css",
    "/css/responsive.css",
    "/java/script.js"
  };

  for (int i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
    if (!SPIFFS.exists(files[i])) {
      Serial.printf("%s not found\n", files[i]);
    }
  }
}
