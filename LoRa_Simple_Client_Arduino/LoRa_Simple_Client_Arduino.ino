#include <ArduinoJson.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"

#define DHTPIN 3
#define DHTTYPE DHT22

RH_RF95 rf95;

float frequency = 868.0;
DHT dht(DHTPIN, DHTTYPE);
float temp = 0.0;
void setup()
{
  Serial.begin(9600);
  dht.begin();
  //while (!Serial) ; // Wait for serial port to be available
  Serial.println("Start LoRa Client");
  if (!rf95.init())
    Serial.println("init failed");

  rf95.setFrequency(frequency);
  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);

}
float getSensorData() {
//  delay(2000);
  float tempdata = dht.readTemperature();
  return tempdata;
//  if (isnan(tempdata)) {
//    return 0.0;
//  }
//  else {
//    return tempdata;
//  }
}
void loop()
{

  String jsonStr;
  StaticJsonDocument <200> doc;

  Serial.println("Sending to LoRa Server");
  //  // Send a message to LoRa Server

  doc["temperature"]      = getSensorData();
  doc["humidity"]         = 80;
  doc["soil_moisture"]    = 70;
  doc["soil_ph"]          = 7.8;
  doc["light_intensity"]  = 900;
  doc["wind_speed"]       = 100;
  doc["wind_direction"]   = "west";

  serializeJson(doc, jsonStr);
  Serial.println(jsonStr);
  uint8_t dataArray[jsonStr.length()];
  jsonStr.toCharArray(dataArray, jsonStr.length() + 1);
  rf95.send(dataArray, sizeof(dataArray));
  //  doc.clear();
  //  json = "";
  //
  rf95.waitPacketSent();
  //  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(2000))
  {
    //    // Should be a reply message for us now
    if (rf95.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is LoRa server running?");
  }
  //  delay(2000);
}
