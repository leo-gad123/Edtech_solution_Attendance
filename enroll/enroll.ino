#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Fingerprint Enrollment System");

  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor ready");
  } else {
    Serial.println("Sensor not found");
    while (1);
  }
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.println("Place finger...");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  if (finger.image2Tz(1) != FINGERPRINT_OK) return p;

  Serial.println("Remove finger...");
  delay(2000);

  while (finger.getImage() != FINGERPRINT_NOFINGER);

  Serial.println("Place same finger again...");

  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  if (finger.image2Tz(2) != FINGERPRINT_OK) return p;

  if (finger.createModel() != FINGERPRINT_OK) {
    Serial.println("Fingerprints did not match");
    return p;
  }

  if (finger.storeModel(id) == FINGERPRINT_OK) {
    Serial.println("Fingerprint stored successfully!");
  } else {
    Serial.println("Error storing fingerprint");
    return p;
  }

  return FINGERPRINT_OK;
}

void loop() {
  Serial.println("\nEnter ID (1-127) to enroll:");
  
  while (!Serial.available());

  id = Serial.parseInt();

  if (id == 0) {
    Serial.println("Invalid ID");
    return;
  }

  Serial.print("Enrolling ID #");
  Serial.println(id);

  getFingerprintEnroll();
}