# ESP32 LED Controller - Installation Guide

## Prerequisites

### Software
- Arduino IDE 2.0+ or PlatformIO
- ESP32 board package

### Hardware  
- ESP32 DevKit v1 (or compatible)
- WS2811/WS2812 LED strip (your existing strip)
- 5V power supply (your existing PSU)
- Level shifter board (your existing shifter)
- Jumper wires

## Arduino IDE Setup

### 1. Install ESP32 Board Package
1. Open Arduino IDE
2. Go to File → Preferences
3. Add this URL to Additional Board Manager URLs:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
   ```
4. Go to Tools → Board → Board Manager
5. Search for "ESP32" and install "ESP32 by Espressif Systems"

### 2. Install Required Libraries
Go to Tools → Manage Libraries and install:
- **FastLED** by Daniel Garcia
- **ArduinoJson** by Benoit Blanchon

### 3. Configure Board Settings
- Board: "ESP32 Dev Module"
- Upload Speed: 921600
- CPU Frequency: 240MHz (WiFi/BT)
- Flash Frequency: 80MHz
- Flash Mode: DIO
- Flash Size: 4MB
- Port: (select your ESP32 port)

## Code Configuration

### 1. Update WiFi Credentials
Edit these lines in `metar_led_controller.ino`:
```cpp
const char* WIFI_SSID = "YourWiFiNetwork";
const char* WIFI_PASSWORD = "YourWiFiPassword";
```

### 2. Verify LED Configuration
```cpp
#define NUM_LEDS 29        // Your LED count
#define LED_PIN 23         // GPIO pin (recommend 23)
#define LED_BRIGHTNESS 90  // Adjust brightness (0-255)
```

### 3. Optional: Change Network Name
```cpp
#define MDNS_NAME "metar-map"  // Access via http://metar-map.local
```

## Upload Process

1. Connect ESP32 to computer via USB
2. Select correct Port in Tools menu
3. Click Upload button
4. Monitor Serial output (Tools → Serial Monitor, 115200 baud)

Expected output:
```
🚀 METAR Map LED Controller Starting...
💡 FastLED initialized: 29 LEDs on GPIO23
🌈 Running LED test pattern...
✅ LED test complete
📶 Connecting to WiFi: YourWiFi ✅
📍 IP Address: 192.168.1.100
🌐 Web server started on port 80
📍 mDNS responder started: http://metar-map.local
✅ LED Controller Ready!
```

## Testing

### 1. Web Interface
Visit `http://metar-map.local` (or the IP shown in Serial Monitor)
- Click "Test LEDs" to verify LED functionality
- Check status information

### 2. API Testing
Use curl or Postman to test the API:

```bash
# Update specific LEDs
curl -X POST http://metar-map.local/update \
  -H "Content-Type: application/json" \
  -d '{
    "leds": [
      {"index": 0, "color": [255, 0, 0]},
      {"index": 1, "color": [0, 255, 0]},
      {"index": 2, "color": [0, 0, 255]}
    ]
  }'

# Get status
curl http://metar-map.local/status

# Run test pattern
curl http://metar-map.local/test
```

## Troubleshooting

### ESP32 Won't Connect to WiFi
- Verify SSID and password are correct
- Check if network is 2.4GHz (ESP32 doesn't support 5GHz)
- Try connecting ESP32 closer to router
- Check Serial Monitor for error messages

### LEDs Don't Light Up
- Verify wiring connections
- Check power supply is adequate (5V, sufficient amperage)
- Ensure level shifter is working (3.3V → 5V)
- Try different GPIO pin if needed
- Check `NUM_LEDS` matches your strip length

### Can't Access Web Interface
- Check IP address in Serial Monitor
- Try IP address directly instead of .local name
- Ensure ESP32 and computer are on same network
- Check firewall settings

### No Response to API Calls
- Verify ESP32 is connected to WiFi
- Check if CORS is needed for your application
- Monitor Serial output for error messages
- Try restarting ESP32

## Wiring Reference

```
ESP32 Pin    →  Connection
GPIO23       →  Level Shifter Input (Data)
5V           →  Level Shifter VCC + LED Strip 5V
GND          →  Level Shifter GND + LED Strip GND

Level Shifter Output  →  LED Strip Data In
```

## Performance Notes

- ESP32 can easily handle 29 LEDs at 60+ FPS
- HTTP requests typically complete in <50ms on local network
- Memory usage is minimal (~200KB free after startup)
- WiFi reconnection is automatic if connection drops