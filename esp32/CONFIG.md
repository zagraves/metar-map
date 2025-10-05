# Configuration System

The ESP32 now loads configuration from a `config.ini` file instead of hardcoded values.

## Setup

1. **Copy the template**:
   ```bash
   cp config.template.ini config.ini
   ```

2. **Edit your settings**:
   ```ini
   [wifi]
   ssid=YOUR_WIFI_NETWORK
   password=YOUR_WIFI_PASSWORD
   
   [device]
   hostname=metar-map
   led_count=29
   led_pin=2
   led_brightness=90
   ```

3. **Upload filesystem** (first time only):
   - Copy your `config.ini` to the `data/` folder
   - In VS Code: `Ctrl+Shift+P` → "PlatformIO: Upload Filesystem Image"
   - Or terminal: `pio run --target uploadfs`

4. **Build and upload code**:
   - `Ctrl+Shift+P` → "PlatformIO: Upload"

## Benefits

- ✅ **No credentials in code**: WiFi passwords stay out of git
- ✅ **Easy configuration**: Change settings without rebuilding
- ✅ **Multiple setups**: Different config files for different environments
- ✅ **Safe defaults**: Falls back to hardcoded values if config is missing

## File Locations

- `config.template.ini` - Template (committed to git)
- `config.ini` - Your settings (ignored by git)
- `data/config.ini` - Uploaded to ESP32 SPIFFS filesystem