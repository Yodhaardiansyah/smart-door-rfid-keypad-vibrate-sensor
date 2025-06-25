# Smart Door RFID and Keypad Vibrate Sensor

An advanced IoT-based smart door system using RFID, PIN keypad, and Telegram for secure access and notification. Built on ESP32 and D1 Mini with dual-microcontroller communication.

## 🔐 Features

- 🔒 RFID-based access (MFRC522)
- 🔢 PIN input using 4x4 Keypad
- 📡 Serverless operation with local validation
- 📲 Telegram alerts on door access
- 🔔 Buzzer and 🖥 LCD feedback
- 🌐 WiFi connectivity
- 🚨 Vibration sensor for tamper detection
- 🔁 Serial communication between ESP32 and D1 Mini

## 📦 Hardware Components

| Component        | Role                         |
|------------------|------------------------------|
| ESP32            | Main controller (logic, RFID, Telegram, relay) |
| D1 Mini          | Keypad handler + buzzer       |
| MFRC522          | RFID reader                   |
| Keypad 4x4       | User PIN input                |
| LCD I2C 16x2     | Feedback display              |
| Buzzer           | Access & alert sound          |
| Relay            | Electric door control         |
| Vibration Sensor | Tamper detection              |

## 🔧 How It Works

![ChatGPT Image 26 Jun 2025, 06 55 30](https://github.com/user-attachments/assets/19f5dafa-b268-46c4-ac1b-1774d995a948)

- D1 Mini membaca input PIN, mengirim ke ESP32 via Serial.
- ESP32 membaca kartu RFID dan mencocokkan UID.
- Jika valid, relay terbuka, buzzer berbunyi, dan Telegram diberi notifikasi.
- Jika sensor getaran aktif saat tidak ada akses valid → trigger alarm.

## 🚀 Getting Started

### 📥 Firmware Installation

#### ESP32
- Libraries needed:
  - `WiFi.h`, `WiFiClientSecure`, `UniversalTelegramBot`
  - `MFRC522`, `LiquidCrystal_I2C`
- Upload `smartdoor2-esp32.ino`

#### D1 Mini
- Libraries needed:
  - `Keypad.h`
- Upload `smartdoor2-d1mini.ino`

### ⚙️ Wiring Notes

- **D1 Mini Keypad Pins:** D1, D2, D3, D4, D5, D6, D7, D8
- **ESP32 RFID Pins:** SDA = GPIO21, RST = GPIO22
- **LCD I2C on ESP32:** SDA = GPIO17, SCL = GPIO16

> ⚠️ Edit `botToken` and `chat_id` in `smartdoor2-esp32.ino`

## 📲 Example Telegram Notification

🔐 Access Granted
User: Yodha
Method: RFID
Time: 08:15:03

shell
Salin
Edit

## 📁 File Structure

Smart_Door_V2/
├── smartdoor2-esp32/
│ └── smartdoor2-esp32.ino
└── smartdoor2-d1mini/
└── smartdoor2-d1mini.ino

csharp
Salin
Edit

## 📄 License

MIT License

## 👤 Author

[Yodha Ardiansyah](https://github.com/Yodhaardiansyah)
