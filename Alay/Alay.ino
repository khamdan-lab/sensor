#include <ArduinoJson.h>

void setup() {
  // put your setup code here, to run once:
  String jsonStr;
  Serial.begin(9600);
  StaticJsonDocument<200> doc;
  doc["sensor"] = "gps";
  doc["time"] = 1351824120;
  doc["khamdan"] = ".";
  serializeJson(doc, jsonStr);
  Serial.println(jsonStr);
}

void loop() {
  // put your main code here, to run repeatedly:

}
