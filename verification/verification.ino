#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

// 📡 Use UART2 (RX=16, TX=17)
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// 👉 Change this to test any ID
#define TEST_ID 1

// 🔧 Function: Check single ID
bool isIDExist(uint8_t id) {
  uint8_t result = finger.loadModel(id);

  if (result == FINGERPRINT_OK) {
    Serial.print("✅ ID ");
    Serial.print(id);
    Serial.println(" exists in sensor");
    return true;
  }

  if (result == FINGERPRINT_BADLOCATION) {
    Serial.print("⚠️ Invalid ID location: ");
    Serial.println(id);
  } 
  else if (result == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("❌ Communication error with sensor");
  } 
  else {
    Serial.print("❌ ID ");
    Serial.print(id);
    Serial.println(" is EMPTY (not registered)");
  }

  return false;
}

// 🔍 Function: Scan all IDs
void scanAllIDs() {
  Serial.println("\n🔎 Scanning all fingerprint IDs...");

  int count = 0;

  for (int i = 1; i <= 127; i++) {
    if (finger.loadModel(i) == FINGERPRINT_OK) {
      Serial.print("✔ Found ID: ");
      Serial.println(i);
      count++;
    }
  }

  if (count == 0) {
    Serial.println("⚠️ No fingerprints stored");
  } else {
    Serial.print("✅ Total IDs found: ");
    Serial.println(count);
  }
}

void setup() {
  Serial.begin(115200);

  // 🔌 Initialize sensor
  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);

  Serial.println("\n🔐 Fingerprint Verification System");

  // ✅ Verify sensor
  if (finger.verifyPassword()) {
    Serial.println("✅ Sensor detected successfully");
  } else {
    Serial.println("❌ Sensor NOT detected");
    while (1);
  }

  // 📊 Show sensor capacity
  Serial.print("📦 Sensor capacity: ");
  Serial.println(finger.capacity);

  Serial.print("👥 Stored templates: ");
  Serial.println(finger.templateCount);

  // 🔍 Check single ID
  Serial.println("\n--- Checking Single ID ---");
  isIDExist(TEST_ID);

  // 🔍 Scan all IDs
  Serial.println("\n--- Scanning All IDs ---");
  scanAllIDs();
}

void loop() {
  // Nothing needed here for verification-only mode
}