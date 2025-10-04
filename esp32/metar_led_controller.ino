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
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// LED configuration  
#define NUM_LEDS 29        // Total number of LEDs in your strip
#define LED_PIN 23         // GPIO pin connected to LED data line
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

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n🚀 METAR Map LED Controller Starting...");
  
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
  MDNS.update();
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED && wifiConnected) {
    Serial.println("⚠️  WiFi connection lost!");
    wifiConnected = false;
    showErrorPattern();
  } else if (WiFi.status() == WL_CONNECTED && !wifiConnected) {
    Serial.println("✅ WiFi reconnected!");
    wifiConnected = true;
    showReadyPattern();
  }
  
  delay(100);
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
  Serial.println("🌈 Running LED test pattern...");
  
  // Sweep blue across all LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
    FastLED.show();
    delay(50);
  }
  
  delay(500);
  
  // Clear all LEDs
  FastLED.clear();
  FastLED.show();
  
  Serial.println("✅ LED test complete");
}

void showReadyPattern() {
  // Brief green flash to indicate ready
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(200);
  FastLED.clear();
  FastLED.show();
}

void showErrorPattern() {
  // Brief red flash to indicate error
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
  delay(200);
  FastLED.clear();
  FastLED.show();
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
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"No JSON body\"}");
    return;
  }
  
  DynamicJsonDocument doc(2048);
  
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    Serial.printf("❌ JSON parse error: %s\n", error.c_str());
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  // Process LED updates (don't clear first to avoid flicker)
  JsonArray ledArray = doc["leds"];
  int updateCount = 0;
  
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
    }
  }
  
  // Update the strip - single atomic operation
  FastLED.show();
  
  // Track statistics
  lastUpdateTime = millis();
  lastLEDCount = updateCount;
  
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
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>METAR Map LED Controller</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }
        .container { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; }
        .status { background: #e8f5e8; padding: 15px; border-radius: 5px; margin: 10px 0; }
        .button { background: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 5px; margin: 5px; cursor: pointer; }
        .button:hover { background: #0056b3; }
        .info { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin: 10px 0; }
        .info div { background: #f8f9fa; padding: 10px; border-radius: 5px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🗺️ METAR Map LED Controller</h1>
        <div class="status">
            <h3>✅ Controller Online</h3>
            <p>Ready to receive LED commands from Raspberry Pi</p>
        </div>
        
        <div class="info">
            <div><strong>LEDs:</strong> )" + String(NUM_LEDS) + R"(</div>
            <div><strong>Pin:</strong> GPIO)" + String(LED_PIN) + R"(</div>
            <div><strong>Brightness:</strong> )" + String(LED_BRIGHTNESS) + R"(</div>
            <div><strong>IP:</strong> )" + WiFi.localIP().toString() + R"(</div>
        </div>
        
        <h3>Test Controls</h3>
        <button class="button" onclick="testLEDs()">🌈 Test LEDs</button>
        <button class="button" onclick="clearLEDs()">⚫ Clear LEDs</button>
        <button class="button" onclick="getStatus()">📊 Get Status</button>
        
        <h3>API</h3>
        <p><strong>POST /update</strong> - Update LED colors</p>
        <p><strong>GET /status</strong> - Get controller status</p>
        <p><strong>GET /test</strong> - Run LED test pattern</p>
    </div>
    
    <script>
        function testLEDs() {
            fetch('/test').then(r => r.text()).then(console.log);
        }
        
        function clearLEDs() {
            fetch('/update', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({leds: []})
            }).then(r => r.json()).then(console.log);
        }
        
        function getStatus() {
            fetch('/status').then(r => r.json()).then(data => {
                alert(JSON.stringify(data, null, 2));
            });
        }
    </script>
</body>
</html>
  )";
  
  server.send(200, "text/html", html);
}

void handleTest() {
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

void handleCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
}