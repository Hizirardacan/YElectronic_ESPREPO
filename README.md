# ESP32_WIFI_plus_Bluetooth (with Bluetooth & Wi-Fi Modules)

This repository demonstrates how to use **ESP32 with AT firmware** in three embedded communication scenarios via UART, driven by an STM32 microcontroller.

Projects included:

1. `ESP32_Bluetooth` – Classic Bluetooth SPP communication  
2. `ESP32-SAMPLE` – Wi-Fi Access Point and TCP server  
3. `ESP32_WIFI_plus_Bluetooth` – Combined Bluetooth + Wi-Fi communication

All modules use **ESP-AT commands** over **UART6**, and send debug output via **UART3**.

---

## 🧠 Common Features

- Based on STM32 and ESP32 UART communication
- AT command interface to control Bluetooth and Wi-Fi
- Built with ESP32 AT firmware (via ESP-IDF)
- C (firmware) using STM32 HAL drivers
- Logs sent to PC via UART3

---

## 📁 Project Breakdown

### 1. `ESP32_Bluetooth`

Classic Bluetooth Serial Port Profile (SPP) communication example.

**Features:**
- Initializes ESP32 Bluetooth stack
- Sets a custom device name
- Scans for other Bluetooth devices
- Enables SPP and starts a connection
- Sends and receives SPP data

**AT Commands Used:**
```
AT+BTINIT=1
AT+BTNAME="ESP32_Bluetooth"
AT+BTSCANMODE=2
AT+BTSPPINIT=2
AT+BTSPPSTART
AT+BTSPPCONN=...
```

---

### 2. `ESP32-SAMPLE`

Wi-Fi Access Point configuration and TCP server example.

**Features:**
- Configures ESP32 in dual AP+STA mode
- Starts a custom AP (SSID: "Name", Password: "123456789")
- Starts a TCP server on port `5000`

**AT Commands Used:**
```
AT+CWMODE=3
AT+CWSAP="Name","123456789",6,3,4,0
AT+CIPMUX=1
AT+CIPSERVER=1,5000
```

---

### 3. `ESP32_WIFI_plus_Bluetooth`

Main project that runs both **Bluetooth SPP** and **Wi-Fi TCP** server at the same time.

**Features:**
- Combines all functionalities of the above two projects
- Initializes BT + Wi-Fi modules via AT commands
- Receives data over:
  - Bluetooth SPP (`+BTDATA`)
  - Wi-Fi TCP (`+IPD`)
- Echoes all received data back:
  - Via `AT+BTSPPSEND` for Bluetooth
  - Via `AT+CIPSEND` for Wi-Fi
- Bluetooth and Wi-Fi can be used simultaneously

**AT Commands Used:**
```
Bluetooth:
AT+BTINIT=1
AT+BTNAME="ESP32_Bluetooth"
AT+BTSCANMODE=2
AT+BTSPPINIT=2
AT+BTSPPSTART
AT+BTSPPCONN=...

Wi-Fi:
AT+CWMODE=3
AT+CWSAP="Name","123456789",6,3,4,0
AT+CIPMUX=1
AT+CIPSERVER=1,5000
```

---

## 🔌 Connections

| Function           | STM32 Peripheral | ESP32 Interface |
|--------------------|------------------|-----------------|
| Bluetooth/Wi-Fi AT | UART6            | UART            |
| Debug/Log Output   | UART3            | (To PC)         |

---

## ▶️ How to Use

1. Flash the STM32 firmware to the board.
2. Connect ESP32 to STM32 via UART6 (RX/TX).
3. Open a serial terminal on your PC connected to UART3.
4. Power on the system.
5. ESP32 will:
   - Start Bluetooth SPP and scan for nearby devices
   - Set up Wi-Fi AP and run a TCP server
6. You can send test messages to ESP32 via:
   - Bluetooth SPP (response sent with `AT+BTSPPSEND`)
   - TCP socket (port 5000, response with `AT+CIPSEND`)

---

## 📦 Dependencies

- STM32 HAL Library
- ESP32 AT Firmware  
  → Get it from: [https://github.com/espressif/esp-at](https://github.com/espressif/esp-at)

---

## 🧾 License

This project is provided under the MIT License. ESP32 AT firmware is licensed by Espressif under their own terms.
