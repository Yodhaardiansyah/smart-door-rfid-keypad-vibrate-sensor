#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

// ===== WIFI & TELEGRAM =====
const char* ssid = "Asui";
const char* password = "123456789";
const char* botToken = "7857292825:AAEOUKssLIhX5csoOmd3ne2VdSaXTQLiZlg"; // Ganti dengan token bot Anda
const String chat_id = "1301633560";   // Ganti dengan chat ID Anda

WiFiClientSecure secured_client;
UniversalTelegramBot bot(botToken, secured_client);

unsigned long lastCheck = 0;
const unsigned long interval = 1000;

// ===== PIN & OBJEK =====
#define RST_PIN         22
#define SS_PIN          21
#define BUZZER_PIN      5
#define RELAY_PIN       0
#define VIBRATION_PIN   4

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte kartu1[4] = {0xAA, 0x52, 0x92, 0x99};

bool alarmAktif = false;
unsigned long getarTime = 0;
int getarCount = 0;
String pasPIN = "1234";

String inputPIN = "";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  Wire.begin(17, 16); // SDA = GPIO17, SCL = GPIO16
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.init();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(VIBRATION_PIN, INPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  // WIFI
  WiFi.begin(ssid, password);
  secured_client.setInsecure();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Menghubungkan ke WiFi...");
  }

  Serial.println("WiFi Terhubung");
  bot.sendMessage(chat_id, "ESP32 Siap - Terkoneksi Internet", "");

  tampilkanSiaga();
}

void loop() {
  handleGetaran();
  handleSerialCommand();
  handleRFID();
  handleTelegram();
}

void handleTelegram() {
  if (millis() - lastCheck > interval) {
    int newMsg = bot.getUpdates(bot.last_message_received + 1);
    while (newMsg) {
      for (int i = 0; i < newMsg; i++) {
        String text = bot.messages[i].text;
        String from = bot.messages[i].chat_id;

        if (from != chat_id) continue;

        if (text == "/bukapintu") {
          bot.sendMessage(chat_id, "Perintah buka pintu dikirim.", "");
          aksesBerhasil("By Telegram");
        } else if (text == "/matikanalarm") {
          bot.sendMessage(chat_id, "Alarm dimatikan.", "");
          alarmAktif = false;
          digitalWrite(BUZZER_PIN, LOW);
          tampilkanSiaga();
        } else {
          bot.sendMessage(chat_id, "Perintah tidak dikenali.", "");
        }
      }
      newMsg = bot.getUpdates(bot.last_message_received + 1);
    }
    lastCheck = millis();
  }
}

void handleGetaran() {
  if (alarmAktif) {
    digitalWrite(BUZZER_PIN, HIGH); // bunyi terus
    return;
  }

  if (digitalRead(VIBRATION_PIN) == HIGH) {
    getarCount++;
    getarTime = millis();

    if (getarCount >= 3) {
      alarmAktif = true;
      bot.sendMessage(chat_id, "NOTIF: Ada penyusupan! Kirim /matikanalarm untuk matikan alarm", "");
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("!!Penyusupan!!");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Deteksi Getar!");
      buzz(3, 100);
      delay(1000);
      tampilkanSiaga();
    }
  }

  if (millis() - getarTime > 5000) getarCount = 0;
}

void handleSerialCommand() {
  while (Serial.available()) {
    String perintah = Serial.readStringUntil('\n');
    perintah.trim();

    if (perintah.startsWith("KEY:")) {
      char key = perintah.charAt(4);
      if (key == '#') {
        if (inputPIN.length() > 0) {
          lcd.clear();
          lcd.setCursor(0, 0); lcd.print("Cek PIN...");
          if (inputPIN == pasPIN) {
            aksesBerhasil("PIN benar");
            bot.sendMessage(chat_id, "PIN benar, pintu dibuka", "");
          } else {
            aksesGagal("PIN salah");
            bot.sendMessage(chat_id, "PIN salah!", "");
          }
          inputPIN = "";
        }
      } else if (key == '*') {
        inputPIN = "";
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("PIN direset");
        delay(1000);
        tampilkanSiaga();
      } else {
        if (inputPIN.length() < 8) {
          inputPIN += key;
          lcd.clear();
          lcd.setCursor(0, 0); lcd.print("Input PIN:");
          lcd.setCursor(0, 1); lcd.print(inputPIN);
        }
      }
    }
  }
}

void handleRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Cek RFID...");

  if (isAuthorized(rfid.uid.uidByte)) {
    aksesBerhasil("RFID benar");
    bot.sendMessage(chat_id, "RFID benar, pintu dibuka", "");
  } else {
    aksesGagal("RFID salah");
    bot.sendMessage(chat_id, "RFID tidak dikenal!", "");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

bool isAuthorized(byte *uid) {
  return (
    memcmp(uid, kartu1, 4) == 0
  );
}

void aksesBerhasil(String msg) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(msg);
  lcd.setCursor(0, 1); lcd.print("Akses dibuka");
  digitalWrite(RELAY_PIN, HIGH);
  buzz(2, 100);
  delay(3000);
  digitalWrite(RELAY_PIN, LOW);
  tampilkanSiaga();
}

void aksesGagal(String msg) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(msg);
  lcd.setCursor(0, 1); lcd.print("Akses ditolak");
  buzz(1, 500);
  delay(2000);
  tampilkanSiaga();
}

void tampilkanSiaga() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("SMART DOOR");
  lcd.setCursor(0, 1); lcd.print("Menunggu input");
}

void buzz(int n, int delayMs) {
  for (int i = 0; i < n; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(delayMs);
    digitalWrite(BUZZER_PIN, LOW);
    delay(delayMs);
  }
}
