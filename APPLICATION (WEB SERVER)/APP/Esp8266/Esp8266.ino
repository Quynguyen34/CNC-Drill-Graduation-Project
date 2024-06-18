#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SoftwareSerial.h>

#define RX_PIN D7  // Define RX pin
#define TX_PIN D8  // Define TX pin

SoftwareSerial espSerial(RX_PIN, TX_PIN);  // Create SoftwareSerial object

// Wi-Fi credentials
const char* wifi_ssid = "UTE.Aruba325";
const char* wifi_password = "vnhcmute";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

bool wifiConnected = false;

uint8_t buffer[256];  // Increase buffer size
int dataIndex = 0;  // Index for the buffer
struct packetData {
  char voltage_data[10];
  char current_data[10];
  char temperature_data[10];
  char power_data[10];
};

struct packetData receivedData;
// Variables to store sensor data
volatile float voltage = 0.0;
volatile float current = 0.0;
volatile float temperature = 0.0;
volatile float power = 0.0;

unsigned long lastMsg = 0;
#define MSG_INTERVAL 100  // Interval in milliseconds for sending messages

void setup() {
  // Initialize serial ports
  Serial.begin(9600);  // Hardware serial for debugging

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount SPIFFS");
    return;
  } else {
    Serial.println("SPIFFS mounted successfully");
  }

  // Check the existence of files in SPIFFS
  checkSPIFFSFiles();

  // Connect to Wi-Fi
  connectToWiFi();

  // Set up routes
  setupRoutes();

  // Start the server
  server.begin();
}

void loop() {
  // Handle UART communication if WiFi is connected
  if (wifiConnected) {
    handleUART();
  }
  // Check WiFi connection status
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
    delay(1000);  // Add a small delay to avoid tight loop
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  wifiConnected = true;
  espSerial.begin(9600);  // Software serial for UART communication
  espSerial.println(WiFi.localIP());
}

void setupRoutes() {
  server.on("/index1.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index1.html", "text/html");
  });

  // Route for index2.html
  server.on("/index2.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index2.html", "text/html");
  });

  // Route for index3.html
  server.on("/index3.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index3.html", "text/html");
  });

  // Default route
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index1.html", "text/html");
  });

  // Routes to send sensor data
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

  // Routes to serve CSS and JS files
  server.on("/css/main.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/main.css", "text/css");
  });
  server.on("/css/base.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/base.css", "text/css");
  });
  server.on("/css/responsive.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/responsive.css", "text/css");
  });
  server.on("/java/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/java/script.js", "text/javascript");
  });
  
  // Routes to handle commands
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

  formattedData += "GOTO" + data.substring(start) + "\n";  // Add the last line

  return formattedData;
}
void handleCommand(AsyncWebServerRequest *request, const String& command) {
  Serial.println(command);
  espSerial.println(command); // Send the command over UART
  request->send(200, "text/plain", "Command sent: " + command);
}

void handleUART() {
  // Check if any data is available to read
  while (espSerial.available()) {
    char c = espSerial.read();  // Read a character

    // If character is newline, packet is complete
    if (c == '\n') {
      buffer[dataIndex] = '\0';  // Null-terminate the string
      Serial.print("Received data: ");
      Serial.println((char*)buffer);  // Debug print

      // Process the received data
      processReceivedData((char*)buffer);

      // Reset index for the next packet
      dataIndex = 0;
      // Clear buffer
      memset(buffer, 0, sizeof(buffer));
    } else {
      // Add character to buffer if not newline
      if (dataIndex < sizeof(buffer) - 1) {
        buffer[dataIndex++] = c;
      } else {
        // Buffer overflow, reset buffer
        dataIndex = 0;
        memset(buffer, 0, sizeof(buffer));
        // Optional, handle buffer overflow
        Serial.println("Buffer overflow, data reset");
      }
    }
  }
}

void processReceivedData(char* data) {
  // Split received data using ',' as delimiter
  Serial.print("Processing data: ");
  Serial.println(data);  // Debug print

  // Use sscanf to parse the received data into the structure
  sscanf(data, "%9[^,],%9[^,],%9[^,],%9[^,]",
         receivedData.voltage_data,
         receivedData.current_data,
         receivedData.temperature_data,
         receivedData.power_data);

  // Convert parsed string data to float and assign to global variables
  voltage = atof(receivedData.voltage_data);
  current = atof(receivedData.current_data);
  temperature = atof(receivedData.temperature_data);
  power = atof(receivedData.power_data);

  // Debug print the parsed values
  Serial.print("Voltage: ");
  Serial.println(voltage);
  Serial.print("Current: ");
  Serial.println(current);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Power: ");
  Serial.println(power);
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
