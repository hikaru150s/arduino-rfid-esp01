#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define ESP_RX 0
#define ESP_TX 1
#define GRANT_PIN 6
#define REVOKE_PIN 7
#define SPI_RST 9
#define SPI_SS 10

SoftwareSerial espSerial(ESP_RX, ESP_TX); // RX, TX for communication with NodeMCU
MFRC522 mfrc522(SPI_SS, SPI_RST); // Define the pins for RFID module (SS, RST)

void setup() {
  pinMode(GRANT_PIN, OUTPUT);
  pinMode(REVOKE_PIN, OUTPUT);
  Serial.begin(9600);
  espSerial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Reader Ready");
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("New RFID present, reading...")''
    String cardData = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardData += String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.print("RFID Data:");
    Serial.println(cardData);
    Serial.println("Sending to Client...");

    // Send data to NodeMCU
    espSerial.println("MARCO:" + cardData);

    // Wait for a response from NodeMCU
    while (!espSerial.available()) {
      delay(10);
    }

    String response;
    do {
      response = espSerial.readStringUntil('\n');
    } while (response.startsWith("POLO:"));
    Serial.print("Got response:");
    Serial.println(response);
    if (response.substring(5) == "true") {
      Serial.println("GRANTED!");
      digitalWrite(GRANT_PIN, HIGH); // Turn on pin 6
      digitalWrite(REVOKE_PIN, LOW);  // Turn off pin 7
    } else {
      Serial.println("REVOKED!");
      digitalWrite(GRANT_PIN, LOW);   // Turn off pin 6
      digitalWrite(REVOKE_PIN, HIGH);  // Turn on pin 7
    }
    delay(5000);
    digitalWrite(GRANT_PIN, LOW);   // Turn off pin 6
    digitalWrite(REVOKE_PIN, LOW);  // Turn off pin 7
    Serial.println("Deactivating, next RFID please...");
  }
}
