#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include "time.h"

// 🔐 WiFi
#define WIFI_SSID "YOUR_WIFI"
#define WIFI_PASSWORD "YOUR_PASSWORD"

// 🔥 Firebase
#define API_KEY "YOUR_API_KEY"
#define DATABASE_URL "https://attendance-80a01-default-rtdb.firebaseio.com/"

// Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Fingerprint
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Time
const char* ntpServer = "pool.ntp.org";

// 📌 Get Date
String getDate() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "0000-00-00";

  char dateStr[11];
  strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);
  return String(dateStr);
}

// 📌 Get Time
String getTimeNow() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "00:00";

  char timeStr[6];
  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
  return String(timeStr);
}

// 🔍 Fingerprint Read
int getFingerprintID() {
  if (finger.getImage() != FINGERPRINT_OK) return -1;
  if (finger.image2Tz() != FINGERPRINT_OK) return -1;
  if (finger.fingerSearch() != FINGERPRINT_OK) return -1;

  return finger.fingerID;
}

void setup() {
  Serial.begin(115200);

  // Fingerprint
  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);

  if (!finger.verifyPassword()) {
    Serial.println("Fingerprint not detected");
    while (1);
  }

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected");

  // Time
  configTime(0, 0, ntpServer);

  // Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  int id = getFingerprintID();

  if (id != -1) {
    Serial.print("User ID: ");
    Serial.println(id);

    String date = getDate();
    String timeNow = getTimeNow();

    String basePath = "/attendance/" + String(id) + "/" + date;

    // 🔍 Check if checkIn exists
    if (Firebase.RTDB.getString(&fbdo, basePath + "/checkIn")) {

      // If checkIn exists → do checkOut
      if (!Firebase.RTDB.getString(&fbdo, basePath + "/checkOut")) {

        if (Firebase.RTDB.setString(&fbdo, basePath + "/checkOut", timeNow)) {
          Serial.println("Check-Out recorded");
        } else {
          Serial.println("Error: " + fbdo.errorReason());
        }

      } else {
        Serial.println("Already checked out today");
      }

    } else {
      // No checkIn → do checkIn
      if (Firebase.RTDB.setString(&fbdo, basePath + "/checkIn", timeNow)) {
        Serial.println("Check-In recorded");
      } else {
        Serial.println("Error: " + fbdo.errorReason());
      }
    }

    delay(4000); // avoid duplicate scans
  }
}