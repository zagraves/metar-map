/*
 * METAR Map LED Controller
 * 
 * Simple WiFi-controlled LED controller for aviation weather displays.
 * Receives color commands via HTTP and controls WS2811/WS2812 LED strips.
 * 
 * Hardware: ESP32 + WS2811 LED strip + 5V PSU + level shifter
 * Communication: HTTP JSON API over WiFi
 */

#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

// ============================================================================
// CONFIGURATION - Update these for your setup
// ============================================================================

// WiFi credentials
const char* WIFI_SSID = "Gravesnet";
const char* WIFI_PASSWORD = "4258020698";

// LED configuration  
#define NUM_LEDS 29        // Total number of LEDs in your strip
#define LED_PIN 2          // GPIO pin connected to LED data line (change to GPIO2 if GPIO23 has issues)
#define LED_BRIGHTNESS 90  // LED brightness (0-255)

// Network configuration
#define MDNS_NAME "metar-map"  // Access via http://metar-map.local
#define HTTP_PORT 80

// ============================================================================
// GLOBALS
// ============================================================================

CRGB leds[NUM_LEDS];
WebServer server(HTTP_PORT);

// Status tracking
unsigned long lastUpdateTime = 0;
int lastLEDCount = 0;
bool wifiConnected = false;

// Metrics tracking
unsigned long ledUpdatesTotal = 0;
unsigned long httpRequestsTotal = 0;
unsigned long httpErrorsTotal = 0;
unsigned long ledUpdateDurationMicros = 0;
int ledsUsed = 0;
int ledsUnused = NUM_LEDS;
unsigned long metricsRequests = 0;
unsigned long statusRequests = 0;
unsigned long testRequests = 0;
unsigned long startTime = 0;

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n🚀 METAR Map LED Controller Starting...");
  
  // Initialize metrics start time
  startTime = millis();
  
  // Initialize LEDs
  setupLEDs();
  
  // Connect to WiFi
  setupWiFi();
  
  // Setup web server
  setupWebServer();
  
  // Setup mDNS
  if (MDNS.begin(MDNS_NAME)) {
    Serial.printf("📍 mDNS responder started: http://%s.local\n", MDNS_NAME);
  }
  
  Serial.println("✅ LED Controller Ready!");
  Serial.printf("📡 IP Address: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("🌐 Web Interface: http://%s.local\n", MDNS_NAME);
  Serial.printf("💡 LED Count: %d\n", NUM_LEDS);
  
  // Show ready pattern
  showReadyPattern();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  server.handleClient();
  // MDNS.update(); // Not available in all ESP32 versions
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    if (wifiConnected) {
      // Just lost connection
      Serial.println("⚠️  WiFi connection lost!");
      wifiConnected = false;
    }
    // Keep pulsing red heartbeat while disconnected
    showErrorPattern();
  } else if (!wifiConnected) {
    // Just reconnected
    Serial.println("✅ WiFi reconnected!");
    wifiConnected = true;
    showReadyPattern();
  }
  
  if (wifiConnected) {
    delay(100);  // Normal delay when connected
  }
  // No extra delay when disconnected - showErrorPattern() has its own timing
}

// ============================================================================
// LED SETUP & PATTERNS
// ============================================================================

void setupLEDs() {
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
  
  Serial.printf("💡 FastLED initialized: %d LEDs on GPIO%d\n", NUM_LEDS, LED_PIN);
  
  // Test pattern on startup
  showStartupPattern();
}

void showStartupPattern() {
  Serial.println("💡 Running LED connectivity test...");
  
  // Sweep white across all LEDs to avoid flight category color confusion
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::White;
    FastLED.show();
    delay(50);
  }
  
  delay(500);
  
  // Clear all LEDs
  FastLED.clear();
  FastLED.show();
  
  Serial.println("✅ LED connectivity test complete");
}

void showReadyPattern() {
  // Heartbeat pulse pattern in white - clearly indicates system is "alive"
  
  // First pulse (strong)
  for (int brightness = 0; brightness <= 255; brightness += 15) {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(8);
  }
  for (int brightness = 255; brightness >= 0; brightness -= 15) {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(8);
  }
  
  delay(100); // Brief pause
  
  // Second pulse (weaker)
  for (int brightness = 0; brightness <= 180; brightness += 15) {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(8);
  }
  for (int brightness = 180; brightness >= 0; brightness -= 15) {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(8);
  }
  
  // Restore original brightness and clear
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
}

void showErrorPattern() {
  // Continuous red heartbeat to indicate error - won't confuse with IFR conditions
  // This keeps pulsing until WiFi reconnects
  
  // First pulse (strong)
  for (int brightness = 0; brightness <= 255; brightness += 20) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(6);
  }
  for (int brightness = 255; brightness >= 0; brightness -= 20) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(6);
  }
  
  delay(80); // Brief pause
  
  // Second pulse (weaker)
  for (int brightness = 0; brightness <= 180; brightness += 20) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(6);
  }
  for (int brightness = 180; brightness >= 0; brightness -= 20) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(6);
  }
  
  // Restore original brightness and clear
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
  
  delay(800); // Longer pause before next heartbeat cycle
}

// ============================================================================
// WIFI SETUP
// ============================================================================

void setupWiFi() {
  Serial.printf("📶 Connecting to WiFi: %s", WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" ✅");
    Serial.printf("📍 IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("📶 Signal Strength: %d dBm\n", WiFi.RSSI());
    wifiConnected = true;
  } else {
    Serial.println(" ❌");
    Serial.println("⚠️  Failed to connect to WiFi!");
    wifiConnected = false;
  }
}

// ============================================================================
// WEB SERVER SETUP
// ============================================================================

void setupWebServer() {
  // LED update endpoint
  server.on("/update", HTTP_POST, handleLEDUpdate);
  
  // Status endpoint
  server.on("/status", HTTP_GET, handleStatus);
  
  // Metrics endpoint (Prometheus format)
  server.on("/metrics", HTTP_GET, handleMetrics);
  
  // Simple web interface
  server.on("/", HTTP_GET, handleRoot);
  
  // Test endpoint
  server.on("/test", HTTP_GET, handleTest);
  
  // Handle CORS preflight
  server.on("/update", HTTP_OPTIONS, handleCORS);
  
  server.begin();
  Serial.printf("🌐 Web server started on port %d\n", HTTP_PORT);
}

// ============================================================================
// HTTP HANDLERS
// ============================================================================

void handleLEDUpdate() {
  unsigned long startMicros = micros();
  httpRequestsTotal++;
  
  if (!server.hasArg("plain")) {
    httpErrorsTotal++;
    server.send(400, "application/json", "{\"error\":\"No JSON body\"}");
    return;
  }
  
  // Calculate required JSON buffer size dynamically
  String jsonString = server.arg("plain");
  size_t jsonSize = jsonString.length();
  
  // Use larger buffer with safety margin (at least 4KB, up to 16KB for large payloads)
  size_t bufferSize = max((size_t)4096, min((size_t)16384, jsonSize * 2));
  
  Serial.printf("📊 JSON size: %d bytes, buffer: %d bytes, free heap: %d\n", 
                jsonSize, bufferSize, ESP.getFreeHeap());
  
  // Check if we have enough free memory
  if (ESP.getFreeHeap() < bufferSize + 8192) {  // Keep 8KB safety margin
    httpErrorsTotal++;
    Serial.printf("❌ Insufficient memory for JSON parsing. Need: %d, Available: %d\n", 
                  bufferSize + 8192, ESP.getFreeHeap());
    server.send(507, "application/json", "{\"error\":\"Insufficient memory\"}");
    return;
  }
  
  DynamicJsonDocument doc(bufferSize);
  
  DeserializationError error = deserializeJson(doc, jsonString);
  if (error) {
    httpErrorsTotal++;
    Serial.printf("❌ JSON parse error: %s (size: %d, buffer: %d)\n", 
                  error.c_str(), jsonSize, bufferSize);
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  // Process LED updates (don't clear first to avoid flicker)
  JsonArray ledArray = doc["leds"];
  int updateCount = 0;
  int currentLedsUsed = 0;
  
  // First, clear only the LEDs that will be updated or set all to black if clearing
  bool shouldClear = doc.containsKey("clear") && doc["clear"];
  if (shouldClear) {
    FastLED.clear();
  }
  
  for (JsonObject led : ledArray) {
    int index = led["index"];
    JsonArray color = led["color"];
    
    if (index >= 0 && index < NUM_LEDS && color.size() == 3) {
      leds[index] = CRGB(color[0], color[1], color[2]);
      updateCount++;
      
      // Count LEDs that are actually lit (not black)
      int r = color[0].as<int>();
      int g = color[1].as<int>();
      int b = color[2].as<int>();
      if (r > 0 || g > 0 || b > 0) {
        currentLedsUsed++;
      }
    }
  }
  
  // Update the strip - single atomic operation
  FastLED.show();
  
  // Update metrics
  ledUpdatesTotal++;
  ledUpdateDurationMicros = micros() - startMicros;
  lastUpdateTime = millis();
  lastLEDCount = updateCount;
  ledsUsed = currentLedsUsed;
  ledsUnused = NUM_LEDS - currentLedsUsed;
  
  Serial.printf("✅ Updated %d LEDs\n", updateCount);
  
  // Send success response
  DynamicJsonDocument response(256);
  response["status"] = "success";
  response["updated"] = updateCount;
  response["timestamp"] = lastUpdateTime;
  
  String responseStr;
  serializeJson(response, responseStr);
  
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", responseStr);
}

void handleStatus() {
  httpRequestsTotal++;
  statusRequests++;
  
  DynamicJsonDocument doc(512);
  
  doc["status"] = wifiConnected ? "online" : "offline";
  doc["uptime"] = millis();
  doc["free_heap"] = ESP.getFreeHeap();
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["wifi_ssid"] = WIFI_SSID;
  doc["led_count"] = NUM_LEDS;
  doc["led_brightness"] = LED_BRIGHTNESS;
  doc["last_update"] = lastUpdateTime;
  doc["last_led_count"] = lastLEDCount;
  doc["ip_address"] = WiFi.localIP().toString();
  
  String response;
  serializeJson(doc, response);
  
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", response);
}

void handleRoot() {
  httpRequestsTotal++;
  
  String html = "<!DOCTYPE html><html><head><title>METAR Map LED Controller</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }";
  html += ".container { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; }";
  html += ".status { background: #e8f5e8; padding: 15px; border-radius: 5px; margin: 10px 0; }";
  html += ".button { background: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 5px; margin: 5px; cursor: pointer; }";
  html += ".button:hover { background: #0056b3; }";
  html += ".info { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin: 10px 0; }";
  html += ".info div { background: #f8f9fa; padding: 10px; border-radius: 5px; }";
  html += "</style></head><body><div class='container'>";
  html += "<h1>🗺️ METAR Map LED Controller</h1>";
  html += "<div class='status'><h3>✅ Controller Online</h3>";
  html += "<p>Ready to receive LED commands from Raspberry Pi</p></div>";
  html += "<div class='info'>";
  html += "<div><strong>LEDs:</strong> " + String(NUM_LEDS) + "</div>";
  html += "<div><strong>Pin:</strong> GPIO" + String(LED_PIN) + "</div>";
  html += "<div><strong>Brightness:</strong> " + String(LED_BRIGHTNESS) + "</div>";
  html += "<div><strong>IP:</strong> " + WiFi.localIP().toString() + "</div>";
  html += "</div><h3>Test Controls</h3>";
  html += "<button class='button' onclick='testLEDs()'>🌈 Test LEDs</button>";
  html += "<button class='button' onclick='clearLEDs()'>⚫ Clear LEDs</button>";
  html += "<button class='button' onclick='getStatus()'>📊 Get Status</button>";
  html += "<button class='button' onclick='getMetrics()'>📈 View Metrics</button>";
  html += "<h3>API</h3>";
  html += "<p><strong>POST /update</strong> - Update LED colors</p>";
  html += "<p><strong>GET /status</strong> - Get controller status</p>";
  html += "<p><strong>GET /metrics</strong> - Prometheus metrics</p>";
  html += "<p><strong>GET /test</strong> - Run LED test pattern</p>";
  html += "</div><script>";
  html += "function testLEDs() { fetch('/test').then(r => r.text()).then(console.log); }";
  html += "function clearLEDs() { fetch('/update', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({clear: true, leds: []}) }).then(r => r.json()).then(console.log); }";
  html += "function getStatus() { fetch('/status').then(r => r.json()).then(data => { alert(JSON.stringify(data, null, 2)); }); }";
  html += "function getMetrics() { window.open('/metrics', '_blank'); }";
  html += "</script></body></html>";
  
  server.send(200, "text/html", html);
}

void handleTest() {
  httpRequestsTotal++;
  testRequests++;
  
  Serial.println("🧪 Running LED test pattern via web request");
  
  // Rainbow pattern
  for (int hue = 0; hue < 256; hue += 5) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(hue + (i * 10), 255, 255);
    }
    FastLED.show();
    delay(50);
  }
  
  // Clear
  FastLED.clear();
  FastLED.show();
  
  server.send(200, "text/plain", "LED test complete");
}

void handleMetrics() {
  httpRequestsTotal++;
  metricsRequests++;
  
  String metrics = "";
  
  // ESP32 system metrics
  metrics += "# HELP esp32_uptime_seconds Total uptime in seconds\n";
  metrics += "# TYPE esp32_uptime_seconds counter\n";
  metrics += "esp32_uptime_seconds " + String((millis() - startTime) / 1000.0) + "\n";
  
  metrics += "# HELP esp32_heap_free_bytes Free heap memory in bytes\n";
  metrics += "# TYPE esp32_heap_free_bytes gauge\n";
  metrics += "esp32_heap_free_bytes " + String(ESP.getFreeHeap()) + "\n";
  
  metrics += "# HELP esp32_wifi_signal_strength_dbm WiFi signal strength in dBm\n";
  metrics += "# TYPE esp32_wifi_signal_strength_dbm gauge\n";
  metrics += "esp32_wifi_signal_strength_dbm " + String(WiFi.RSSI()) + "\n";
  
  metrics += "# HELP esp32_wifi_connected WiFi connection status (1=connected, 0=disconnected)\n";
  metrics += "# TYPE esp32_wifi_connected gauge\n";
  metrics += "esp32_wifi_connected " + String(wifiConnected ? 1 : 0) + "\n";
  
  // LED metrics
  metrics += "# HELP leds_total Total number of LEDs in strip\n";
  metrics += "# TYPE leds_total gauge\n";
  metrics += "leds_total " + String(NUM_LEDS) + "\n";
  
  metrics += "# HELP leds_used Number of LEDs currently lit (non-black)\n";
  metrics += "# TYPE leds_used gauge\n";
  metrics += "leds_used " + String(ledsUsed) + "\n";
  
  metrics += "# HELP leds_unused Number of LEDs currently unused (black/off)\n";
  metrics += "# TYPE leds_unused gauge\n";
  metrics += "leds_unused " + String(ledsUnused) + "\n";
  
  metrics += "# HELP led_brightness Current LED brightness setting (0-255)\n";
  metrics += "# TYPE led_brightness gauge\n";
  metrics += "led_brightness " + String(LED_BRIGHTNESS) + "\n";
  
  metrics += "# HELP led_updates_total Total number of LED update requests\n";
  metrics += "# TYPE led_updates_total counter\n";
  metrics += "led_updates_total " + String(ledUpdatesTotal) + "\n";
  
  metrics += "# HELP led_update_duration_microseconds Duration of last LED update in microseconds\n";
  metrics += "# TYPE led_update_duration_microseconds gauge\n";
  metrics += "led_update_duration_microseconds " + String(ledUpdateDurationMicros) + "\n";
  
  // HTTP metrics
  metrics += "# HELP http_requests_total Total HTTP requests by endpoint\n";
  metrics += "# TYPE http_requests_total counter\n";
  metrics += "http_requests_total{endpoint=\"update\"} " + String(ledUpdatesTotal) + "\n";
  metrics += "http_requests_total{endpoint=\"status\"} " + String(statusRequests) + "\n";
  metrics += "http_requests_total{endpoint=\"metrics\"} " + String(metricsRequests) + "\n";
  metrics += "http_requests_total{endpoint=\"test\"} " + String(testRequests) + "\n";
  metrics += "http_requests_total{endpoint=\"root\"} " + String(httpRequestsTotal - ledUpdatesTotal - statusRequests - metricsRequests - testRequests) + "\n";
  
  metrics += "# HELP http_errors_total Total HTTP errors\n";
  metrics += "# TYPE http_errors_total counter\n";
  metrics += "http_errors_total " + String(httpErrorsTotal) + "\n";
  
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain; version=0.0.4; charset=utf-8", metrics);
}

void handleCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
}