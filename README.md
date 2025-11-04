# QR-Queue Lite

> A lightweight, edge-first queue management system for campus canteens using ESP32, MQTT, and WebSocket technologies.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-brightgreen.svg)](https://www.arduino.cc/)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/praventhegenius/qr-queue-lite/pulls)

## Overview

QR-Queue Lite is an IoT-based queue management solution designed specifically for high-traffic canteen environments. Unlike traditional cloud-dependent systems, this project leverages edge computing principles by running entirely on an ESP32 microcontroller, providing:

- **Zero Internet Dependency** - Operates on local WiFi network
- **Real-Time Updates** - Sub-100ms latency via WebSocket protocol
- **App-Free Design** - Pure web-based interface, no downloads required
- **Cost-Effective** - Complete system under $15
- **Self-Contained** - Creates its own WiFi access point

Originally developed as part of the BCSE308L Computer Networks and IoT Laboratory course, this system demonstrates practical implementation of multiple networking protocols (HTTP, WebSocket, MQTT) in a real-world edge computing scenario.

---

## Table of Contents

- [Features](#features)
- [System Architecture](#system-architecture)
- [Technology Stack](#technology-stack)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [API Documentation](#api-documentation)
- [Project Structure](#project-structure)
- [Performance Metrics](#performance-metrics)
- [Troubleshooting](#troubleshooting)
- [Future Enhancements](#future-enhancements)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)
- [Contact](#contact)

---

## Features

### Core Functionality

- **Token Management System**
  - Automatic token issuance with sequential numbering
  - Real-time position tracking for students
  - Persistent queue state across device restarts

- **Multi-Interface Design**
  - Student Interface: Request and monitor tokens
  - Display Board: Large-screen token visibility
  - Staff Panel: Queue control and management

- **Real-Time Synchronization**
  - WebSocket-based instant updates
  - MQTT pub/sub architecture for event handling
  - Broadcast updates to all connected clients simultaneously

- **Offline-First Architecture**
  - Local WiFi Access Point mode
  - No external server dependencies
  - Works without internet connectivity

### Technical Highlights

- **Edge Computing**: All processing happens on ESP32, reducing latency and improving reliability
- **Asynchronous I/O**: Non-blocking server architecture supports multiple concurrent connections
- **SPIFFS Integration**: Web files stored in flash memory for fast serving
- **Responsive Design**: Mobile-optimized interface works on any device with a browser
- **Protocol Stack**: Demonstrates HTTP, WebSocket, and MQTT working in harmony

---

## System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    ESP32 Microcontroller                │
│                                                         │
│  ┌────────────────────────────────────────────────┐     │
│  │           WiFi Access Point Layer              │     │
│  │    SSID: QR-Queue-Canteen | IP: 192.168.4.1    │     │
│  └────────────────────────────────────────────────┘     │
│                         ↓                               │
│  ┌────────────────────────────────────────────────┐     │
│  │              Services Layer                    │     │
│  │  ┌──────────────┐  ┌──────────────┐            │     │
│  │  │ MQTT Broker  │  │  Web Server  │            │     │
│  │  │ Port: 1883   │  │  Port: 80    │            │     │
│  │  │ (PicoMQTT)   │  │ (AsyncWeb)   │            │     │
│  │  └──────────────┘  └──────────────┘            │     │ 
│  │  ┌──────────────┐  ┌──────────────┐            │     │ 
│  │  │  WebSocket   │  │    SPIFFS    │            │     │
│  │  │ Endpoint:/ws │  │  File System │            │     │
│  │  └──────────────┘  └──────────────┘            │     │ 
│  └────────────────────────────────────────────────┘     │
│                         ↓                               │
│  ┌────────────────────────────────────────────────┐     │
│  │           Application Layer                    │     │
│  │  Token Logic | Queue Management | API Handlers │     │
│  └────────────────────────────────────────────────┘     │
└─────────────────────────────────────────────────────────┘
                         ↓
        ┌────────────────┼────────────────┐
        ↓                ↓                ↓
   [Students]      [Display TV]      [Staff Panel]
```

### Communication Flow

1. **Token Request Flow**
   ```
   Student → HTTP POST /api/get_token → ESP32 increments counter 
   → Returns token #N → Student receives confirmation
   ```

2. **Queue Advancement Flow**
   ```
   Staff clicks "Next" → HTTP POST /api/next_token 
   → MQTT publish "canteen/line/next" → Subscription callback 
   → current_token++ → WebSocket broadcast → All clients update
   ```

3. **Real-Time Update Flow**
   ```
   WebSocket connection → Token update event → JSON broadcast 
   → All connected clients receive update → UI refresh (<100ms)
   ```

---

## Technology Stack

### Hardware
- **Microcontroller**: ESP32-WROOM-32 (dual-core, WiFi enabled)
- **Memory**: 520 KB SRAM, 4 MB Flash
- **Connectivity**: WiFi 802.11 b/g/n (2.4 GHz)

### Software & Protocols
- **Programming Language**: C++ (Arduino Framework)
- **Networking Protocols**: 
  - HTTP/1.1 (REST API)
  - WebSocket (RFC 6455)
  - MQTT 3.1.1 (Pub/Sub messaging)
  - TCP/IP Stack
- **File System**: SPIFFS (SPI Flash File System)
- **Front-End**: HTML5, CSS3, Vanilla JavaScript

### Libraries & Dependencies
- **ESPAsyncWebServer** v3.x - Asynchronous HTTP server
- **AsyncTCP** - TCP library for async operations
- **PicoMQTT** - Lightweight MQTT broker/client
- **ArduinoJson** v6.x - JSON serialization/deserialization
- **WiFi.h** - ESP32 WiFi management
- **SPIFFS.h** - Flash file system interface

---

## Hardware Requirements

### Essential Components
| Component | Specification | Quantity | Estimated Cost |
|-----------|--------------|----------|----------------|
| ESP32 Development Board | ESP32-WROOM-32 | 1 | $6-10 |
| USB Cable | Micro-USB or USB-C (data transfer) | 1 | $2-3 |
| Power Supply | 5V 1A minimum | 1 | $3-5 |

### Optional Components
- **Physical Button**: Tactile push button for hardware "Next" trigger
- **Display Module**: SSD1306 OLED for standalone token display
- **Enclosure**: 3D printed or commercial project box
- **LED Indicators**: Status lights for visual feedback

### Display Devices (User-Provided)
- **Student Access**: Any smartphone, tablet, or laptop with WiFi and web browser
- **Display Board**: TV/Monitor with HDMI input or WiFi-capable display
- **Staff Control**: Smartphone, tablet, or dedicated device

---

## Software Requirements

### Development Environment
- **Arduino IDE**: Version 2.0 or higher
- **ESP32 Board Package**: Version 2.x or 3.x
- **Operating System**: Windows 10+, macOS 10.15+, or Linux (Ubuntu 20.04+)

### Required Arduino Libraries
Install via Arduino Library Manager or manual installation:

```
ESPAsyncWebServer by ESP32Async (v3.x)
AsyncTCP by ESP32Async (v1.x)
PicoMQTT (latest from GitHub)
ArduinoJson by Benoit Blanchon (v6.x)
```

### SPIFFS Upload Tool
- **Arduino IDE 2.x**: Install `arduino-spiffs-upload` plugin
- Download: [espx-cz/arduino-spiffs-upload](https://github.com/espx-cz/arduino-spiffs-upload/releases)
- Place `.vsix` file in `~/.arduinoIDE/plugins/`

---

## Installation

### Step 1: Clone the Repository

```bash
git clone https://github.com/praventhegenius/qr-queue-lite.git
cd qr-queue-lite
```

### Step 2: Install Arduino IDE and ESP32 Support

1. Download and install [Arduino IDE 2.0+](https://www.arduino.cc/en/software)
2. Add ESP32 board support:
   - Open Arduino IDE
   - Navigate to **File → Preferences**
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools → Board → Boards Manager**
   - Search "ESP32" and install "ESP32 by Espressif Systems"

### Step 3: Install Required Libraries

#### Method A: Library Manager (Recommended)
1. Open **Sketch → Include Library → Manage Libraries**
2. Search and install:
   - `ESPAsyncWebServer by ESP32Async`
   - `AsyncTCP by ESP32Async`
   - `ArduinoJson by Benoit Blanchon`

#### Method B: Manual Installation (PicoMQTT)
1. Download [PicoMQTT](https://github.com/mlesniew/PicoMQTT)
2. Click **Code → Download ZIP**
3. In Arduino IDE: **Sketch → Include Library → Add .ZIP Library**
4. Select the downloaded ZIP file

### Step 4: Install SPIFFS Upload Tool

1. Download the latest `.vsix` file from [arduino-spiffs-upload releases](https://github.com/espx-cz/arduino-spiffs-upload/releases)
2. Locate Arduino IDE plugins folder:
   - **Windows**: `C:\Users\<Username>\.arduinoIDE\plugins\`
   - **macOS**: `~/.arduinoIDE/plugins/`
   - **Linux**: `~/.arduinoIDE/plugins/`
3. Copy the `.vsix` file into the plugins folder
4. Restart Arduino IDE

### Step 5: Configure Board Settings

1. Connect ESP32 via USB
2. In Arduino IDE, select:
   ```
   Tools → Board → ESP32 Dev Module
   Tools → Upload Speed → 115200
   Tools → Flash Size → 4MB (32Mb)
   Tools → Partition Scheme → Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)
   Tools → Port → [Select your ESP32 port]
   ```

### Step 6: Upload SPIFFS Files

1. Open `QR_Queue_Lite_ESP32.ino` in Arduino IDE
2. Ensure `data/` folder contains all HTML/CSS files
3. Press **Cmd+Shift+P** (macOS) or **Ctrl+Shift+P** (Windows/Linux)
4. Type "spiffs" and select "Upload SPIFFS to Pico/ESP8266/ESP32"
5. Wait for upload completion (~30 seconds)

### Step 7: Upload Arduino Sketch

1. Click the **Upload** button or press **Cmd+U** / **Ctrl+U**
2. Wait for compilation and upload to complete
3. Open Serial Monitor (115200 baud) to verify successful startup

### Step 8: Verify Installation

Upon successful upload, the Serial Monitor should display:

```
=== QR-Queue Lite Starting ===
✓ SPIFFS mounted successfully
✓ AP IP address: 192.168.4.1
✓ MQTT Broker started on port 1883
✓ WebSocket handler registered
✓ Web Server started

=== System Ready ===
WiFi SSID: QR-Queue-Canteen
WiFi Password: canteen123
Web Interface: http://192.168.4.1
```

---

## Configuration

### WiFi Settings

Edit these constants in `QR_Queue_Lite_ESP32.ino`:

```cpp
const char* ap_ssid = "QR-Queue-Canteen";     // Change network name
const char* ap_password = "canteen123";        // Change password (min 8 chars)
```

### Queue Management

Adjust initial token values:

```cpp
int current_token = 0;           // Starting token number
int next_token_to_issue = 1;     // First token to issue
```

### Network Configuration

Modify MQTT and Web Server ports if needed:

```cpp
PicoMQTT::ServerLocalSubscribe mqtt(1883);  // MQTT broker port
AsyncWebServer server(80);                   // Web server port
```

### MQTT Topics

Default topics (can be customized):

```cpp
#define MQTT_TOPIC_NEXT    "canteen/line/next"
#define MQTT_TOPIC_CURRENT "canteen/line/current"
```

---

## Usage

### For Students

1. **Connect to WiFi**
   - Network: `QR-Queue-Canteen`
   - Password: `canteen123`

2. **Get Token**
   - Scan QR code or navigate to: `http://192.168.4.1/get`
   - Click "Get My Token"
   - Note your token number and position in queue

3. **Monitor Status**
   - View live "Currently Serving" number
   - Your position updates automatically as queue progresses
   - Receive alert when your token is called

### For Display Setup (TV/Monitor)

1. Connect display device to WiFi network
2. Open browser and navigate to: `http://192.168.4.1/display`
3. Enable full-screen mode (F11 on most browsers)
4. Display will auto-update as staff advances queue

### For Staff

1. **Access Control Panel**
   - Connect to WiFi: `QR-Queue-Canteen`
   - Navigate to: `http://192.168.4.1/staff`

2. **Advance Queue**
   - Click "CALL NEXT TOKEN" button
   - All displays update instantly
   - Alternative: Press spacebar for quick advance

3. **Monitor Statistics**
   - View current token being served
   - See next token in queue
   - Track total people waiting
   - Monitor tokens issued

---

## API Documentation

### REST Endpoints

#### POST /api/get_token
Issues a new queue token to a student.

**Request**
```http
POST /api/get_token HTTP/1.1
Content-Type: application/json
```

**Response**
```json
{
  "token": 12,
  "current_token": 5,
  "position": 7,
  "timestamp": 123456789
}
```

#### POST /api/next_token
Advances the queue to the next token (staff only).

**Request**
```http
POST /api/next_token HTTP/1.1
Content-Type: application/json
```

**Response**
```json
{
  "status": "success",
  "message": "Token advanced",
  "current_token": 6
}
```

#### GET /api/status
Retrieves current system status.

**Request**
```http
GET /api/status HTTP/1.1
```

**Response**
```json
{
  "current_token": 6,
  "next_token": 13,
  "queue_length": 6,
  "timestamp": 123456789
}
```

### WebSocket Protocol

**Endpoint**: `ws://192.168.4.1/ws`

**Message Format**
```json
{
  "type": "token_update",
  "current_token": 6,
  "next_token": 13,
  "timestamp": 123456789
}
```

**Events**
- `token_update`: Broadcast when queue advances
- Connection automatically sends current state on connect

### MQTT Topics

| Topic | Direction | QoS | Description |
|-------|-----------|-----|-------------|
| `canteen/line/next` | Publish | 1 | Trigger queue advancement |
| `canteen/line/current` | Subscribe | 0 | Current token updates |

---

## Project Structure

```
QR_Queue_Lite_ESP32/
│
├── QR_Queue_Lite_ESP32.ino      # Main Arduino sketch
│   ├── WiFi Access Point setup
│   ├── MQTT broker configuration
│   ├── Web server routes
│   ├── WebSocket handlers
│   └── API endpoint implementations
│
├── data/                         # SPIFFS web files
│   ├── get_token.html           # Student interface
│   │   ├── Token request form
│   │   ├── Position display
│   │   └── WebSocket client
│   │
│   ├── display.html             # Display board interface
│   │   ├── Large token display
│   │   ├── Auto-refresh logic
│   │   └── WebSocket listener
│   │
│   ├── staff.html               # Staff control panel
│   │   ├── Next token button
│   │   ├── Queue statistics
│   │   └── Control interface
│   │
│   └── style.css                # Shared styling
│       ├── Responsive design
│       ├── Color schemes
│       └── Animations
│
│
├── README.md                    # This file
├── LICENSE                      # MIT License
└── .gitignore                  # Git ignore rules
```

---

## Performance Metrics

### Measured Performance

| Metric | Value | Test Conditions |
|--------|-------|----------------|
| Update Latency | 50-150ms | Staff click to display update |
| Token Issue Time | <100ms | Request to response |
| Concurrent Users | 10-15 devices | Simultaneous WebSocket connections |
| WiFi Range | 30-50 meters | Indoor environment |
| Uptime | 24/7 capable | With stable power supply |
| Power Consumption | ~500mW | Average during operation |
| Memory Usage | 45KB / 327KB | RAM utilization |
| Flash Usage | 1.08MB / 1.31MB | Program storage |

### Scalability

- **Single ESP32**: Handles 10-15 concurrent users comfortably
- **Multiple Counters**: Deploy separate ESP32 units per counter
- **Network Load**: Minimal bandwidth (<1 Kbps per connection)

---

## Troubleshooting

### Common Issues

#### Problem: ESP32 not detected
**Solution:**
- Check USB cable supports data transfer (not just charging)
- Install CP2102 or CH340 drivers if needed
- Try different USB port or cable
- Press and hold BOOT button while connecting

#### Problem: SPIFFS upload fails
**Solution:**
- Ensure Serial Monitor is closed
- Verify `data/` folder exists in sketch directory
- Check partition scheme includes SPIFFS
- Restart Arduino IDE and try again

#### Problem: WiFi network not visible
**Solution:**
- Verify ESP32 powered properly (LED should be lit)
- Wait 10 seconds after power-on for AP to start
- Check WiFi is enabled on client device
- Try 2.4GHz-only WiFi settings on phone

#### Problem: WebSocket not updating
**Solution:**
- Verify WebSocket connection in browser console
- Check all clients connected to same network
- Ensure MQTT subscription is working (check Serial Monitor)
- Refresh browser and reconnect

#### Problem: "SPIFFS Mount Failed"
**Solution:**
- Re-upload SPIFFS files using upload tool
- Verify partition scheme includes SPIFFS
- Check flash size is 4MB
- Re-upload Arduino sketch after SPIFFS

### Debug Mode

Enable verbose logging by adding to `setup()`:

```cpp
Serial.setDebugOutput(true);
```

Monitor output at 115200 baud for detailed diagnostics.

---

## Future Enhancements

### Planned Features

- **Multi-Counter Support**: Coordinate multiple ESP32 units
- **Token Cancellation**: Allow students to leave queue
- **Priority Tokens**: VIP or staff tokens with higher priority
- **Analytics Dashboard**: Queue metrics and historical data
- **SMS Notifications**: Alert students via SMS when close to turn
- **NFC Integration**: Physical card tap for token issuance
- **Database Logging**: Store queue history in SQLite
- **PWA Support**: Install as Progressive Web App
- **Multi-Language**: Internationalization support

### Technical Improvements

- **OTA Updates**: Over-the-air firmware updates
- **Mesh Networking**: ESP-MESH for multi-device coordination
- **Bluetooth LE**: Alternative connectivity option
- **Security Enhancements**: SSL/TLS for WebSocket, authentication
- **Load Balancing**: Distribute connections across multiple ESP32s

### Community Requests

Submit feature requests via [GitHub Issues](https://github.com/praventhegenius/qr-queue-lite/issues)

---

## Contributing

Contributions are welcome! Please follow these guidelines:

### How to Contribute

1. **Fork the Repository**
   ```bash
   git fork https://github.com/praventhegenius/qr-queue-lite.git
   ```

2. **Create Feature Branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make Changes**
   - Follow existing code style
   - Comment complex logic
   - Test thoroughly on hardware

4. **Commit Changes**
   ```bash
   git commit -m "Add: Brief description of changes"
   ```

5. **Push to Fork**
   ```bash
   git push origin feature/your-feature-name
   ```

6. **Submit Pull Request**
   - Describe changes in detail
   - Reference related issues
   - Include test results

### Code Style Guidelines

- Use meaningful variable names
- Comment non-obvious logic
- Keep functions focused and concise
- Follow Arduino coding conventions
- Test on actual ESP32 hardware before PR

### Reporting Bugs

Submit bug reports via [GitHub Issues](https://github.com/praventhegenius/qr-queue-lite/issues) with:
- ESP32 board model
- Arduino IDE version
- Library versions
- Steps to reproduce
- Expected vs actual behavior
- Serial Monitor output

---

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

### MIT License Summary

```
Copyright (c) 2025 Praven Raamshankar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, subject to the following conditions:

[Full license text in LICENSE file]
```

---

## Acknowledgments

### Libraries & Frameworks

- **ESPAsyncWebServer** by me-no-dev - Asynchronous web server framework
- **PicoMQTT** by mlesniew - Lightweight MQTT broker implementation
- **ArduinoJson** by Benoit Blanchon - Efficient JSON library
- **ESP32 Arduino Core** by Espressif - Arduino framework for ESP32

### Inspirations & References

- Edge computing principles for IoT applications
- MQTT pub/sub messaging patterns
- WebSocket real-time communication protocols
- Progressive Web App design patterns

---

## Contact

### Project Maintainer

**Pravenraam Shankar**

### Get in Touch

- **Email**: [dev.pravenraamshankar@gmail.com](mailto:dev.pravenraamshankar@gmail.com)
- **LinkedIn**: [linkedin.com/in/pravenraam](https://www.linkedin.com/in/pravenraam/)
- **GitHub**: [github.com/praventhegenius](https://github.com/praventhegenius)
- **Bento**: [bento.me/praven](https://bento.me/praven)


### Project Status

**Status**: Active Development  
**Version**: 1.0.0  
**Last Updated**: November 2025  
**Maintenance**: Actively maintained

---

<div align="center">

### Star this repository if you found it helpful!

**Built with passion for edge computing and IoT innovation**

[Report Bug](https://github.com/praventhegenius/qr-queue-lite/issues) · [Request Feature](https://github.com/praventhegenius/qr-queue-lite/issues) · [Documentation](https://github.com/praventhegenius/qr-queue-lite/wiki)

</div>
