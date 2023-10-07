#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

const char* ssid = "SSID";
const char* password = "password";
const char *url = "http://192.168.229.52:8000/api/read-card";
SoftwareSerial unoSerial(1, 2);

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(9600);
  unoSerial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Standby mode");
}

void loop() {
  if (unoSerial.available() && WiFi.status() == WL_CONNECTED) {
    String data = unoSerial.readString();
    if (data.startsWith("MARCO:")) {
      Serial.println("Receiving:" + data);
      http.begin(client, url);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpCode = http.POST("member_id=" + data.substring(6));
      if (httpCode > 0) {
        String payload = http.getString();
        // parse the payload and determine the result
        Serial.println("Response:" + payload);
        bool result = parsePayload(payload);
        String strResult = result ? "true" : "false";
        unoSerial.print("POLO:");
        unoSerial.println(strResult);
        Serial.println("Result:" + strResult);
      }
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
