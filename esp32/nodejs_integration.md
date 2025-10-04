# Node.js Integration Example

Example code for integrating the ESP32 LED controller with your existing Node.js METAR system.

## Add to your existing scan.mjs

```javascript
import axios from 'axios';

// Configuration
const ESP32_BASE_URL = 'http://metar-map.local'; // or use IP address
const ESP32_TIMEOUT = 5000; // 5 second timeout

/**
 * Send LED color updates to ESP32 controller
 * @param {Array} lightAssignments - Array of light assignments from your existing logic
 */
async function updateESP32Display(lightAssignments) {
    // Convert your existing light assignments to ESP32 format
    const payload = {
        leds: lightAssignments.map(assignment => ({
            index: assignment.ledIndex,
            color: [assignment.red, assignment.green, assignment.blue]
        }))
    };

    try {
        const response = await axios.post(`${ESP32_BASE_URL}/update`, payload, {
            timeout: ESP32_TIMEOUT,
            headers: {
                'Content-Type': 'application/json'
            }
        });

        console.log(`✅ Updated ESP32 display: ${response.data.updated} LEDs`);
        return true;

    } catch (error) {
        console.error('❌ Failed to update ESP32 display:', error.message);
        
        // Optional: Fall back to direct LED control if ESP32 is unavailable
        // return updateLEDsDirectly(lightAssignments);
        
        return false;
    }
}

/**
 * Check ESP32 controller status
 */
async function checkESP32Status() {
    try {
        const response = await axios.get(`${ESP32_BASE_URL}/status`, {
            timeout: ESP32_TIMEOUT
        });

        console.log('📊 ESP32 Status:', {
            status: response.data.status,
            uptime: Math.floor(response.data.uptime / 1000) + 's',
            wifi_rssi: response.data.wifi_rssi + 'dBm',
            free_heap: Math.floor(response.data.free_heap / 1024) + 'KB'
        });

        return response.data;

    } catch (error) {
        console.error('❌ ESP32 status check failed:', error.message);
        return null;
    }
}

/**
 * Test ESP32 LED functionality
 */
async function testESP32LEDs() {
    try {
        await axios.get(`${ESP32_BASE_URL}/test`, {
            timeout: 10000 // Longer timeout for test pattern
        });

        console.log('✅ ESP32 LED test completed');
        return true;

    } catch (error) {
        console.error('❌ ESP32 LED test failed:', error.message);
        return false;
    }
}

// Export functions for use in your existing code
export { updateESP32Display, checkESP32Status, testESP32LEDs };
```

## Integration with your existing METAR processing

Modify your existing scan logic to send updates to ESP32:

```javascript
// In your existing METAR processing function
async function processMetarData(metarData) {
    // Your existing logic to process METAR and determine flight categories
    const lightAssignments = await processFlightCategories(metarData);
    
    // NEW: Send updates to ESP32 instead of (or in addition to) direct LED control
    const esp32Success = await updateESP32Display(lightAssignments);
    
    if (!esp32Success) {
        // Fallback: use your existing direct LED control
        console.log('🔄 Falling back to direct LED control');
        await updateLEDsDirectly(lightAssignments);
    }
    
    return lightAssignments;
}
```

## Startup checks

Add ESP32 health check to your startup routine:

```javascript
// Add to your startup sequence
async function initializeSystem() {
    console.log('🚀 Initializing METAR Map System...');
    
    // Your existing initialization
    await loadConfigurations();
    
    // NEW: Check ESP32 connectivity
    const esp32Status = await checkESP32Status();
    if (esp32Status) {
        console.log('✅ ESP32 LED controller connected');
        
        // Optional: Run LED test on startup
        await testESP32LEDs();
    } else {
        console.log('⚠️  ESP32 LED controller not available - using direct control');
    }
    
    // Continue with your existing startup
    await startMetarPolling();
}
```

## Configuration options

Add ESP32 settings to your config files:

```json
// In your config/default.json or similar
{
    "esp32": {
        "enabled": true,
        "baseUrl": "http://metar-map.local",
        "timeout": 5000,
        "fallbackToDirectControl": true,
        "statusCheckInterval": 300000
    }
}
```

```javascript
// Use configuration in your code
const config = require('./config/default.json');

const ESP32_BASE_URL = config.esp32.baseUrl;
const ESP32_TIMEOUT = config.esp32.timeout;
const ESP32_ENABLED = config.esp32.enabled;
```

## Error handling and retry logic

```javascript
async function updateESP32WithRetry(lightAssignments, maxRetries = 3) {
    for (let attempt = 1; attempt <= maxRetries; attempt++) {
        try {
            const success = await updateESP32Display(lightAssignments);
            if (success) return true;
            
        } catch (error) {
            console.log(`🔄 ESP32 update attempt ${attempt}/${maxRetries} failed:`, error.message);
            
            if (attempt < maxRetries) {
                // Wait before retry (exponential backoff)
                await new Promise(resolve => setTimeout(resolve, 1000 * attempt));
            }
        }
    }
    
    console.log('❌ All ESP32 update attempts failed');
    return false;
}
```

## Monitoring and logging

```javascript
// Add to your monitoring/logging system
async function logESP32Metrics() {
    const status = await checkESP32Status();
    if (status) {
        // Log metrics to your monitoring system
        console.log('📊 ESP32 Metrics:', {
            uptime: status.uptime,
            heap: status.free_heap,
            wifi: status.wifi_rssi,
            last_update: status.last_update
        });
    }
}

// Run periodic health checks
setInterval(logESP32Metrics, 5 * 60 * 1000); // Every 5 minutes
```

This integration approach allows you to:
- Keep all your existing METAR parsing and configuration logic
- Add ESP32 LED control with minimal code changes  
- Maintain fallback capability to direct LED control
- Monitor ESP32 health and performance
- Gradually migrate functionality as needed