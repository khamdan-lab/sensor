#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <RH_RF95.h>

#define LDRPIN A0
#define DHTPIN 13

#define DHTTYPE    DHT22     // DHT 22 (AM2302)
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;


RH_RF95 rf95;

float frequency = 868.0;


void setup() {
  Serial.begin(9600);
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  
  Serial.println("Start LoRa Client");
  if (!rf95.init())
    Serial.println("init failed");
  rf95.setFrequency(frequency);
  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);
 

}

void loop() {
  
  Serial.println("Sending to LoRa Server");
  
  StaticJsonDocument <200> doc;
  StaticJsonDocument<200> jsonBuffer;
  sensors_event_t event;
  
  
  dht.temperature().getEvent(&event);
  AnalogValue = analogRead(LDRPIN);
//
//  const char* cahaya = doc["cahaya"];
//  double suhu  = doc["suhu"];

  doc["cahaya"] = AnalogValue;
  doc["suhu"] = event.temperature;
  uint8_t json[100] = doc;

//  uint8_t json[] =  "{\"sensor_id\":\"gps_1245\",\"reg_time\":110520191445,\"sensor_direction\":[48.756080,2.302038]}";
  
//  rf95.send(json, sizeof(json));
  rf95.waitPacketSent();
  
  delay(6000);
  

}
