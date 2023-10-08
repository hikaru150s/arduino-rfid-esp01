#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define ESP_RX 2
#define ESP_TX 3
#define LOCK_RELAY 4
#define BUZZER 5
#define GRANT_PIN 6
#define REVOKE_PIN 7
#define SPI_RST 9
#define SPI_SS 10

SoftwareSerial espSerial(ESP_RX, ESP_TX); // RX, TX for communication with NodeMCU
MFRC522 mfrc522(SPI_SS, SPI_RST); // Define the pins for RFID module (SS, RST)

void setup() {
  pinMode(GRANT_PIN, OUTPUT);
  pinMode(REVOKE_PIN, OUTPUT);
  pinMode(LOCK_RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  espSerial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  digitalWrite(LOCK_RELAY, HIGH);
  digitalWrite(BUZZER, LOW);
  Serial.println("Reader Ready");
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("New RFID present, reading...");
    String cardData = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      String byteRead = String(mfrc522.uid.uidByte[i], HEX);
      cardData += byteRead.length() > 1 ? byteRead : ("0" + byteRead);
    }
    Serial.print("RFID Data:");
    Serial.println(cardData);
    Serial.println("Sending to Client...");

    // Send data to NodeMCU
    espSerial.println("MARCO:" + cardData);

    String response;
    do {
      // Wait for a response from NodeMCU
      while (!espSerial.available()) {
        delay(10);
      }
      response = espSerial.readStringUntil('\n');
      Serial.println("Client response:" + response);
    } while (!response.startsWith("POLO:"));
    Serial.print("Got response:");
    Serial.println(response);
    if (response.substring(5) == "true") {
      Serial.println("GRANTED!");
      digitalWrite(GRANT_PIN, HIGH);
      digitalWrite(LOCK_RELAY, LOW);
      digitalWrite(REVOKE_PIN, LOW);
    } else {
      Serial.println("REVOKED!");
      digitalWrite(GRANT_PIN, LOW);
      digitalWrite(LOCK_RELAY, HIGH);
      digitalWrite(REVOKE_PIN, HIGH);
      digitalWrite(BUZZER, HIGH);
    }
    delay(5000);
    digitalWrite(GRANT_PIN, LOW);
    digitalWrite(REVOKE_PIN, LOW);
    digitalWrite(LOCK_RELAY, HIGH);
    digitalWrite(BUZZER, LOW);
    Serial.println("Deactivating, next RFID please...");
  }
}
