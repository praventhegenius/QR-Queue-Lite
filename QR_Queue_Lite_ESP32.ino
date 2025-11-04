#include <PicoMQTT.h>

#include <AsyncTCP.h>

/*
 * QR-Queue Lite - ESP32 Implementation
 * Web-based token queue system for campus canteen
 * 
 * Features:
 * - Access Point mode (creates own WiFi network)
 * - MQTT Broker (PicoMQTT)
 * - Web Server (ESPAsyncWebServer)
 * - WebSocket for real-time updates
 * - SPIFFS for storing web files
 */

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <PicoMQTT.h>
#include <ArduinoJson.h>

// ===== WiFi Access Point Configuration =====
const char* ap_ssid = "QR-Queue-Canteen";
const char* ap_password = "canteen123";

// ===== MQTT Configuration =====
PicoMQTT::ServerLocalSubscribe mqtt(1883);  // MQTT broker with local subscriptions


// ===== Web Server & WebSocket =====
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ===== Queue Management Variables =====
int current_token = 0;      // Currently being served
int next_token_to_issue = 1; // Next token to be issued
unsigned long last_update = 0;

// ===== Function Declarations =====
void setupWiFi();
void setupMQTT();
void setupWebServer();
void setupWebSocket();
void handleTokenRequest(AsyncWebServerRequest *request);
void handleNextToken(AsyncWebServerRequest *request);
void handleStatus(AsyncWebServerRequest *request);
void broadcastTokenUpdate();
String processor(const String& var);

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== QR-Queue Lite Starting ===");

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("ERROR: SPIFFS Mount Failed");
    return;
  }
  Serial.println("✓ SPIFFS mounted successfully");

  // Setup WiFi Access Point
  setupWiFi();

  // Setup MQTT Broker
  setupMQTT();

  // Setup Web Server & WebSocket
  setupWebSocket();
  setupWebServer();

  // Start Web Server
  server.begin();
  Serial.println("✓ Web Server started");

  Serial.println("\n=== System Ready ===");
  Serial.println("WiFi SSID: " + String(ap_ssid));
  Serial.println("WiFi Password: " + String(ap_password));
  Serial.println("Web Interface: http://192.168.4.1");
  Serial.println("Get Token: http://192.168.4.1/get");
  Serial.println("Display Board: http://192.168.4.1/display");
  Serial.println("Staff Panel: http://192.168.4.1/staff");
  Serial.println("========================\n");
}

// ===== MAIN LOOP =====
void loop() {
  mqtt.loop();  // Handle MQTT
  ws.cleanupClients();  // Cleanup WebSocket connections
  delay(10);
}

// ===== WiFi Setup =====
void setupWiFi() {
  Serial.println("Setting up WiFi Access Point...");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("✓ AP IP address: ");
  Serial.println(IP);
}

// ===== MQTT Setup =====
void setupMQTT() {
  Serial.println("Setting up MQTT Broker...");
  
  // Subscribe to "next" topic - when staff presses next button
  mqtt.subscribe("canteen/line/next", [](const char * topic, const char * payload) {
    Serial.println("MQTT: Received 'next' command");
    current_token++;
    
    // Broadcast update to all WebSocket clients
    broadcastTokenUpdate();
    
    Serial.println("Token advanced to: " + String(current_token));
  });
  
  mqtt.begin();
  Serial.println("✓ MQTT Broker started on port 1883");
}


// ===== WebSocket Setup =====
void setupWebSocket() {
  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, 
                AwsEventType type, void *arg, uint8_t *data, size_t len) {

    if (type == WS_EVT_CONNECT) {
      Serial.printf("WebSocket client #%u connected\n", client->id());

      // Send current status to newly connected client
      StaticJsonDocument<200> doc;
      doc["type"] = "token_update";
      doc["current_token"] = current_token;
      doc["next_token"] = next_token_to_issue;
      doc["timestamp"] = millis();

      String response;
      serializeJson(doc, response);
      client->text(response);

    } else if (type == WS_EVT_DISCONNECT) {
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
    }
  });

  server.addHandler(&ws);
  Serial.println("✓ WebSocket handler registered");
}

// ===== Web Server Setup =====
void setupWebServer() {
  Serial.println("Setting up Web Server routes...");

  // Root redirects to /get
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("/get");
  });

  // Serve HTML pages from SPIFFS
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/get_token.html", "text/html");
  });

  server.on("/display", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/display.html", "text/html");
  });

  server.on("/staff", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/staff.html", "text/html");
  });

  // Serve static files (CSS, JS)
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // API Endpoints
  server.on("/api/get_token", HTTP_POST, handleTokenRequest);
  server.on("/api/next_token", HTTP_POST, handleNextToken);
  server.on("/api/status", HTTP_GET, handleStatus);

  // 404 Handler
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Not Found");
  });
}

// ===== API Handler: Get Token =====
void handleTokenRequest(AsyncWebServerRequest *request) {
  int user_token = next_token_to_issue;
  next_token_to_issue++;

  int position = user_token - current_token;
  if (position < 0) position = 0;

  // Create JSON response
  StaticJsonDocument<200> doc;
  doc["token"] = user_token;
  doc["current_token"] = current_token;
  doc["position"] = position;
  doc["timestamp"] = millis();

  String response;
  serializeJson(doc, response);

  request->send(200, "application/json", response);

  Serial.println("Token issued: #" + String(user_token) + 
                 " (Position: " + String(position) + ")");
}

// ===== API Handler: Next Token =====
void handleNextToken(AsyncWebServerRequest *request) {
  // Publish to MQTT topic - this will trigger the subscription handler
  mqtt.publish("canteen/line/next", "next");

  // Create JSON response
  StaticJsonDocument<200> doc;
  doc["status"] = "success";
  doc["message"] = "Token advanced";
  doc["current_token"] = current_token;

  String response;
  serializeJson(doc, response);

  request->send(200, "application/json", response);
}

// ===== API Handler: Status =====
void handleStatus(AsyncWebServerRequest *request) {
  StaticJsonDocument<200> doc;
  doc["current_token"] = current_token;
  doc["next_token"] = next_token_to_issue;
  doc["queue_length"] = next_token_to_issue - current_token - 1;
  doc["timestamp"] = millis();

  String response;
  serializeJson(doc, response);

  request->send(200, "application/json", response);
}

// ===== Broadcast Token Update via WebSocket =====
void broadcastTokenUpdate() {
  StaticJsonDocument<200> doc;
  doc["type"] = "token_update";
  doc["current_token"] = current_token;
  doc["next_token"] = next_token_to_issue;
  doc["timestamp"] = millis();

  String message;
  serializeJson(doc, message);

  ws.textAll(message);

  Serial.println("Broadcasted update via WebSocket");
}
