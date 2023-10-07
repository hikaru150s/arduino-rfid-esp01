#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "SSID";
const char* password = "password";
const char *url = "http://192.168.229.52:8000/api/read-card";

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Standby mode");
}

void loop() {
  if (Serial.available() && WiFi.status() == WL_CONNECTED) {
    String data = Serial.readString();
    if (data.startsWith("MARCO:")) {
      http.begin(client, url);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpCode = http.POST("member_id=" + data.substring(6));
      if (httpCode > 0) {
        String payload = http.getString();
        // parse the payload and determine the result
        bool result = parsePayload(payload);
        Serial.print("POLO:");
        Serial.println(result ? "true" : "false");
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
