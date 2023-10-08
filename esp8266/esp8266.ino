#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

const char* ssid = "ABDUL";
const char* password = "@Sira1212";
const char *url = "http://192.168.100.7:8000/api/read-card";
SoftwareSerial unoSerial(5, 4);

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(9600);
  unoSerial.begin(9600);
  Serial.println("Booting...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Standby mode");
}

void loop() {
  if (unoSerial.available() && WiFi.status() == WL_CONNECTED) {
    String data = unoSerial.readStringUntil('\n');
    Serial.println("Receiving:" + data);
    if (data.startsWith("MARCO:")) {
      String id = data.substring(6);
      Serial.println("ID:" + id);
      http.begin(client, url);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpCode = http.POST("card_number=" + id);
      Serial.print("HTTP Code:");
      Serial.println(httpCode);
      if (httpCode > 0) {
        String payload = http.getString();
        // parse the payload and determine the result
        Serial.println("Response:" + payload);
        bool result = parsePayload(payload);
        String strResult = result ? "true" : "false";
        unoSerial.print("POLO:" + strResult);
        Serial.println("Result:" + strResult);
      } else {
        unoSerial.print("POLO:false");
      }
      Serial.println("Closing...");
      http.end();
    }
  }
}

bool parsePayload(String payload) {
  StaticJsonDocument<128> doc;
  deserializeJson(doc, payload);
  const char* result = doc["status"];
  const int stat = strcmp(result, "Active");
  return stat == 0;
}
