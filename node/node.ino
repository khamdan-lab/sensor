#include <ArduinoUniqueID.h>
#include <SPI.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <RH_RF95.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
#include <Wire.h>

#define RAINFALL 3
#define DHTPIN 4
#define LDRPIN A0
#define PHTANAHPIN A1
#define WINDSPEED A2
#define SOILMOISTURE A3
#define PHAIR A4
#define DHTTYPE DHT22
#define RXPin 6
#define TXPin 5
#define GPSBaud 9600



RH_RF95 rf95;
DHT dht(DHTPIN, DHTTYPE);

TinyGPS gps;
SoftwareSerial gpsSerial(RXPin, TXPin);
float lat , lon ;
String uid;
float frequency = 868.0;
byte tipingAmount ;
void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  dht.begin();
  Wire.begin();
  pinMode(RAINFALL, INPUT);
  attachInterrupt(digitalPinToInterrupt(RAINFALL), tipingCount, FALLING);
  Serial.println("Start LoRa Client");
  if (!rf95.init())
    Serial.println("init failed");

  rf95.setFrequency(frequency);
  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);

  for (byte i = 0; i < UniqueIDsize; i++)
  {
    if (UniqueID[i] < 0x10)
      uid += "0" ;
    uid.concat(UniqueID[i]);
    uid += " ";
  }

}


void loop()
{
  sendData();

}

float getTemperature() {
  float tempdata = dht.readTemperature();
  float nan;
  if (isnan(tempdata) ) {
    float temp = 0.00;
    return temp;
  } else {
    return tempdata;
  }
}

float getHumidity() {
  float tempdata = dht.readHumidity();
  if (isnan(tempdata) ) {
    float temp = 0.00;
    return temp;
  } else {
    return tempdata;
  }
}

int getLightIntensity() {
  byte AnalogValue;
  AnalogValue = analogRead(LDRPIN);
  if (isnan(AnalogValue) ) {
    int temp = 0;
    return temp;
  } else {
    return AnalogValue;
  }

}

float getPH() {
  byte sensorValue = 0;
  float outputValue = 0.0;
  sensorValue = analogRead(PHTANAHPIN);
  if (sensorValue >= 450 && sensorValue <= 470) {
    outputValue = 0.0;
    return outputValue;
  }
  else {
    outputValue = (-0.0139 * sensorValue) + 7.7851;
    return outputValue ;
  }
}

float getPHWater() {
  byte adcPH = analogRead(PHAIR);
  float voltage = adcPH * 5.0 / 1022;
  float pHValue = (6.4516 * voltage) - 5.7742;
  return pHValue;

}

int getSoilMoisture() {
  byte sensorValue = 0;
  byte percentValue = 0;
  sensorValue = analogRead(SOILMOISTURE);
  if (sensorValue >= 350 && sensorValue <= 365  ) {
    int temp = 0;
    return temp;
  } else {
    percentValue = map(sensorValue, 1023, 465, 0, 100);
    return percentValue;
  }
}

String getLatitude() {
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      gps.f_get_position(&lat, &lon);
    }
  }
  String latitude = String(lat, 2);
  return latitude;
  delay(500);
}

String getLongitude() {
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      gps.f_get_position(&lat, &lon);
    }
  }
  String longitude = String(lon, 2);
  return longitude;
  delay(500);
}

void sendData() {
  String jsonStr;
  StaticJsonDocument <300> doc;

  doc["id"]      = uid;
  doc["lat"]     = getLatitude();
  doc["lon"]     = getLongitude();
  doc["temp"]    = getTemperature();
  doc["humd"]    = getHumidity();
  doc["som"]     = getSoilMoisture();
  doc["sph"]     = getPH();
  doc["wph"]     = getPHWater();
  doc["light"]   = getLightIntensity();
  doc["ws"]      = getWindSpeed();
  doc["rain"]    = getRain();

  serializeJson(doc, jsonStr);
  Serial.println(jsonStr);
  uint8_t dataArray[jsonStr.length()];
  jsonStr.toCharArray(dataArray, jsonStr.length() + 1);
  rf95.send(dataArray, sizeof(dataArray));
  delay(1000);
}

void tipingCount() {

  tipingAmount++;
}

float getRain() {
  
  float rainIntensity ;
  rainIntensity = 363.636 * tipingAmount;
  return rainIntensity;
}

float getWindSpeed() {
  byte sensorValue = analogRead(WINDSPEED);
  float voltageConversionConstant = .004882814;

  float windSpeed = 0;
  float voltageMin = .4;
  float windSpeedMin = 0;
  float voltageMax = 2.0;
  float windSpeedMax = 32;
  
 float sensorVoltage = sensorValue * voltageConversionConstant;

  if (sensorVoltage <= voltageMin) {
    windSpeed = 0;
    return windSpeed;
  } else {
    windSpeed = (sensorVoltage - voltageMin) * windSpeedMax / (voltageMax - voltageMin);
    return windSpeed;
  }
}
