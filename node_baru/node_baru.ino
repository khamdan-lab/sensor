#include <ArduinoUniqueID.h>
#include <SPI.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>

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
#define frequency 868.0


RH_RF95 rf95;
DHT dht(DHTPIN, DHTTYPE);

TinyGPS gps;
SoftwareSerial gpsSerial(RXPin, TXPin);
String uid;
byte tipingAmount;

void setup() {
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
  delay(1000);
}


void loop()
{
    String a, b, c, d, e, f, g, h, i, j = "";
    a.concat(getTemperature());
    b.concat(getHumidity());
    c.concat(getSoilMoisture());
    d.concat(getPH());
    e.concat(getPHWater());
    f.concat(getLightIntensity());
    g.concat(getWindSpeed());
    h.concat(getRain());
    i.concat(getLatitude());
    j.concat(getLongitude());
  
    String formatData = uid +","+ a + "," + b + "," + c + "," + d + "," + e + "," + f + "," + g + "," + h +","+ i +","+ j;
    String formatOled =  a + "," + b + "," + c + "," + d + "," + e + "," + f + "," + g + "," + h +"}";
    char gelOne[100];
    char gelTwo[100];
    formatData.toCharArray(gelOne, formatData.length() + 1);
    formatOled.toCharArray(gelTwo, formatOled.length() + 1);
    Wire.beginTransmission(8);
    rf95.send(gelOne, sizeof(gelOne));
    Wire.write(gelTwo, sizeof(gelTwo));
    Serial.println(gelOne);
    Serial.println(gelTwo);
    Wire.endTransmission();
//  Serial.println(getLightIntensity());
//  delay(100); 

}

float getTemperature() {
  if (isnan(dht.readTemperature())) {
    return 0.00;
  } else {
    return dht.readTemperature();
  }
}

float getHumidity() {
  if (isnan(dht.readHumidity())) {
    return 0.00;
  } else {
    return dht.readHumidity();
  }
}

int getLightIntensity() {
  int AnalogValue = analogRead(LDRPIN);
  if (isnan(AnalogValue) ) {
    return 0;
  } else {
    return AnalogValue;
  }
}

float getPH() {
  int sensorValue = 0;
  float outputValue = 0.0;
  sensorValue = analogRead(PHTANAHPIN);
  float rumus = (-0.0139 * sensorValue) + 7.7851;
  Serial.println(sensorValue);
  Serial.println(rumus);
  
//  if (sensorValue >= 450 && sensorValue <= 470) {
//    return 0.0;
//  }
//  else {
//    outputValue = (-0.0139 * sensorValue) + 7.7851;
//    return outputValue ;
//  }
}

float getPHWater() {
  byte adcPH = analogRead(PHAIR);
  float voltage = adcPH * 5.0 / 1022;
  float pHValue = (6.4516 * voltage) - 5.7742;
  return pHValue;
}

int getSoilMoisture() {
  int sensorValue = 0;
  int percentValue = 0;
  int AirValue = 607;
  int WaterValue = 267;

  sensorValue = analogRead(SOILMOISTURE);
  percentValue = map(sensorValue, AirValue, WaterValue, 0, 100);
  if (percentValue >= 100)
  {
    return 100;
  }
  else if (percentValue <= 0)
  {
    return 0;
  }
  else if (percentValue > 0 && percentValue < 100)
  {
    return percentValue;
  }
}

String getLatitude() {
  float lat , lon ;
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
  float lat , lon ;
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      gps.f_get_position(&lat, &lon);
    }
  }
  String longitude = String(lon, 2);
  return longitude;
  delay(500);
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
    return 0;
  } else {
    windSpeed = (sensorVoltage - voltageMin) * windSpeedMax / (voltageMax - voltageMin);
    return windSpeed;
  }
}
