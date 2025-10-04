# METAR Map Hybrid Architecture Plan

## 🎯 Project Overview

**Objective**: Create a hybrid system where Raspberry Pi handles METAR processing and ESP32 provides dedicated LED control via WiFi.

**Strategy**: Leverage strengths of both platforms while solving LED timing issues.

**Timeline**: 2-3 weeks  
**Complexity**: Low (minimal changes to existing system)  
**Risk Level**: Very Low (existing system remains fully functional)

## 🏗️ Architecture Design

```
┌─────────────────────┐    WiFi/HTTP    ┌─────────────────────┐
│   Raspberry Pi      │ ◄─────────────► │   ESP32 Controller │
│   (Node.js Master)  │                 │   (LED Slave)       │
├─────────────────────┤                 ├─────────────────────┤
│ • METAR parsing     │                 │ • LED control only  │
│ • Flight categories │                 │ • HTTP web server   │
│ • Configuration     │                 │ • FastLED library   │
│ • Scheduling        │                 │ • Status reporting  │
│ • Error handling    │                 │ • Simple patterns   │
│ • Future features   │                 │ • mDNS discovery    │
└─────────────────────┘                 └─────────────────────┘
```

## ✅ Benefits of Hybrid Approach

### **Raspberry Pi Advantages (Keep)**
- **Proven METAR parsing** - Your `fboes/metar-parser` + `mathjs` system works perfectly
- **Complex configuration** - JSON configs with expressions already tuned
- **Network reliability** - Robust HTTP handling, retries, error management  
- **Rich ecosystem** - Can add web interface, logging, analytics easily
- **Future expansion** - Multiple displays, OTA updates, remote monitoring
- **Development speed** - Node.js rapid iteration for new features

### **ESP32 Advantages (Add)**  
- **Dedicated LED timing** - No OS interference with WS2811 protocol
- **Reliable startup** - Boots in seconds, waits for Pi commands
- **Remote placement** - Only needs WiFi, can be anywhere behind map
- **Simple maintenance** - Minimal code, easy to troubleshoot
- **Clean installation** - Only ESP32 board + wiring behind map
- **Cost effective** - $8 ESP32 vs complex Pi relocation

## 📡 Communication Protocol

### **Simple HTTP JSON API**
```javascript
// Pi sends every 5 minutes:
POST http://metar-map.local/update
{
  "leds": [
    {"index": 1, "color": [0, 255, 0]},    // KHQM: VFR
    {"index": 3, "color": [0, 0, 255]},    // KSHN: MVFR  
    {"index": 4, "color": [255, 0, 0]}     // KOLM: IFR
  ]
}
```

### **Status Monitoring**
```javascript
// Pi can check ESP32 health:
GET http://metar-map.local/status
{
  "status": "online",
  "uptime": 12345,
  "wifi_rssi": -45,
  "led_count": 29,
  "free_heap": 200000
}
```

## 🚀 Implementation Plan

### **Phase 1: ESP32 Setup (Week 1)**
- [ ] **Hardware preparation**:
  - [ ] Flash ESP32 with LED controller firmware
  - [ ] Test LED connectivity and timing
  - [ ] Verify WiFi connection and HTTP API
  - [ ] Confirm mDNS discovery works
- [ ] **Software validation**:
  - [ ] Test API endpoints with curl/Postman
  - [ ] Verify LED patterns and colors
  - [ ] Check status reporting functionality
  - [ ] Test error recovery scenarios

### **Phase 2: Node.js Integration (Week 2)**  
- [ ] **Minimal code changes**:
  - [ ] Add HTTP client function to send LED updates
  - [ ] Modify existing light assignment logic to call ESP32
  - [ ] Add ESP32 health checking to startup sequence
  - [ ] Implement fallback to direct LED control if ESP32 unavailable
- [ ] **Testing and validation**:
  - [ ] Compare LED timing with original system
  - [ ] Verify all flight categories display correctly
  - [ ] Test network failure scenarios and recovery
  - [ ] Validate 5-minute update cycle reliability

### **Phase 3: Deployment & Monitoring (Week 3)**
- [ ] **Production deployment**:
  - [ ] Install ESP32 behind map in final position
  - [ ] Update Pi configuration to use ESP32 by default
  - [ ] Add monitoring and logging for ESP32 communication
  - [ ] Document new system for future maintenance
- [ ] **Optimization and polish**:
  - [ ] Fine-tune LED brightness and colors if needed
  - [ ] Add web interface for ESP32 status and control
  - [ ] Implement any additional features (test patterns, etc.)
  - [ ] Create backup/restore procedures

## 🔧 Technical Implementation

### **ESP32 Code (Complete)**
- **File**: `esp32/metar_led_controller.ino` 
- **Features**: HTTP server, FastLED control, status reporting, test patterns
- **Dependencies**: FastLED, ArduinoJson, WiFi libraries
- **Size**: ~200 lines of Arduino code

### **Node.js Changes (Minimal)**
```javascript
// Add to existing scan.mjs:
async function updateESP32Display(lightAssignments) {
    const payload = {
        leds: lightAssignments.map(light => ({
            index: light.ledIndex,
            color: [light.red, light.green, light.blue]
        }))
    };
    
    try {
        await fetch('http://metar-map.local/update', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });
        console.log('✅ Updated ESP32 display');
    } catch (error) {
        console.error('❌ ESP32 update failed:', error);
        // Fall back to direct LED control
        await updateLEDsDirectly(lightAssignments);
    }
}
```

## 🎯 Success Criteria

### **Primary Goals**
- [ ] LED timing jitter eliminated (smooth, consistent colors)
- [ ] System reliability improved (ESP32 dedicated hardware)
- [ ] All existing functionality preserved (flight categories, scheduling)
- [ ] Easy maintenance and troubleshooting

### **Secondary Goals**  
- [ ] Cleaner physical installation (Pi can be remote from map)
- [ ] Foundation for future enhancements (multiple displays, web interface)
- [ ] Better error recovery and status monitoring
- [ ] Simplified LED hardware debugging

## 🔄 Migration Strategy

### **Gradual Rollout**
1. **Parallel operation** - ESP32 runs alongside existing LED control
2. **A/B testing** - Compare LED performance between systems  
3. **Confidence building** - Verify reliability over several days
4. **Full cutover** - Switch to ESP32 as primary LED controller
5. **Cleanup** - Remove direct LED control code from Pi

### **Rollback Plan**
- ESP32 failure → Automatic fallback to direct Pi LED control
- Network issues → Pi continues METAR processing, displays local status
- Complete rollback → Simply disconnect ESP32, Pi resumes full control

## 📈 Future Enhancements

With hybrid architecture in place, easy to add:
- **Multiple displays** - One Pi controlling several ESP32 map displays
- **Web dashboard** - Real-time status, historical weather trends  
- **Remote monitoring** - Email/SMS alerts for system issues
- **Advanced patterns** - Animated weather transitions, startup sequences
- **OTA updates** - Remote ESP32 firmware updates via Pi

## 💰 Cost Analysis

**Additional Hardware**: ~$8 (ESP32 DevKit v1)  
**Development Time**: 2-3 weeks part-time  
**Risk Level**: Very Low (existing system unchanged)  
**ROI**: High (eliminates LED timing issues, enables future expansion)

This hybrid approach acknowledges that your Node.js METAR system works perfectly and focuses on solving the specific LED timing problem with minimal risk and maximum benefit.