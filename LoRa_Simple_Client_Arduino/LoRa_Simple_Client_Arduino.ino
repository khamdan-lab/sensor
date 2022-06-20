#include <ArduinoUniqueID.h>
#include <SPI.h>
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <RH_RF95.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"


#define DHTPIN 3
#define LDRPIN A0
#define PHTANAHPIN A1
#define SOILMOISTURE A3
#define WINDSPEED A2
#define DHTTYPE DHT22
#define RXPin 5
#define TXPin 6
#define GPSBaud 9600

int color = NULL;

RH_RF95 rf95;
DHT dht(DHTPIN, DHTTYPE);

TinyGPS gps;
SoftwareSerial gpsSerial(RXPin, TXPin);
float lat ,lon ;

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     4 
#define SCREEN_ADDRESS 0x3c
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SSD1306AsciiAvrI2c display;
String uid;
float frequency = 868.0;


void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  dht.begin();
  display.begin(&Adafruit128x64, SCREEN_ADDRESS);
  display.setFont(utf8font10x16);
  Serial.println("Start LoRa Client");
  if (!rf95.init())
    Serial.println("init failed");

  rf95.setFrequency(frequency);
  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);

//  UniqueIDdump(Serial);
  for (size_t i = 0; i < UniqueIDsize; i++)
  {
    if (UniqueID[i] < 0x10)
      uid += "0" ;
    uid.concat(UniqueID[i]);
    uid += " ";
  }
//    displayOled();
  
}


void loop()
{
//    sendData();
//  Serial.println(getTemperature());
//  Serial.print(",");
//  Serial.println(getHumidity());
//  smartDelay(1000);
message();
delay(10000);

}
float getTemperature() {
  float tempdata = dht.readTemperature();
  float nan;
  if (isnan(tempdata) ){
    float temp = 0.00;
    return temp;    
  }else{
    return tempdata;
  }
   
  
}

float getHumidity() {
  float tempdata = dht.readHumidity();
  if (isnan(tempdata) ){
    float temp = 0.00;
    return temp;    
  }else{
    return tempdata;
  }
}

int getLightIntensity() {
  unsigned int AnalogValue;
  AnalogValue = analogRead(LDRPIN);
  if (isnan(AnalogValue) ){
    int temp = 0;
    return temp;    
  }else{
    return AnalogValue;
  }
  
}

float getPH() {
  int sensorValue = 0;        
  float outputValue = 0.0;
  
  sensorValue = analogRead(PHTANAHPIN);
  outputValue = (-0.0139*sensorValue)+7.7851;
  return outputValue;
}


void message(){
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char *)buf);
    }
    else
    {
      Serial.println("recv failed");
    }
}

int getSoilMoisture(){
  
  int sensorValue = 0;
  int percentValue = 0;
  sensorValue = analogRead(SOILMOISTURE);
  percentValue = map(sensorValue, 1023, 465, 0, 100);
  if (isnan(percentValue) ){
    int temp = 0;
    return temp;    
  }else{
    return percentValue;
  }
  
}

String getLatitude(){
  while(gpsSerial.available()){ 
    if(gps.encode(gpsSerial.read())){
      gps.f_get_position(&lat,&lon);
    }
   }    
    String latitude = String(lat,2);
    return latitude;
}

String getLongitude(){
  while(gpsSerial.available()){ 
    if(gps.encode(gpsSerial.read())){
      gps.f_get_position(&lat,&lon);
    }
   }    
    String longitude = String(lon,2);
    return longitude;
}
        


static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}

void displayOled(){
  display.clear();
  display.setCursor(0, 0);
  display.print("Temp :");
  display.print(getTemperature());
  display.print(" ");
  display.println("C");
  
  display.setCursor(0, 10);
  display.print("Humd :");
  display.print(getHumidity());
  display.print(" ");
  display.println("%");

  display.setCursor(0, 30);
  display.print("PH Soil:");
  display.println(7);

  display.setCursor(0, 40);
  display.print("PH Water:");
  display.println(7.8);
  
  delay(5000);
  display.clear();

  display.setCursor(0, 10);
  display.print("Soil Moisture :");
  display.print(getSoilMoisture());
  display.print(" ");
  display.println("%");

  display.setCursor(0, 20);
  display.print("Light Intensity:");
  display.println(getLightIntensity());

  display.setCursor(0, 30);
  display.print("Wind Speed:");
  display.print(500);
  display.print(" ");
  display.println("m/s");
  
  display.setCursor(0, 40);
  display.print("Latitude:");
  display.println(getLatitude());
  

  delay(5000);
 
}

void sendData(){
  String jsonStr;
  StaticJsonDocument <300> doc;
  
  doc["id"]      = uid;
  doc["lat"]     = getLatitude();
  doc["lon"]     = getLongitude();
  doc["temp"]    = getTemperature();
  doc["humd"]    = getHumidity();
  doc["som"]     = getSoilMoisture();
  doc["sph"]     = 7.8;
  doc["wph"]     = 7.8;
  doc["light"]   = getLightIntensity();
  doc["ws"]      = 100;
  
  serializeJson(doc, jsonStr);
  Serial.println(jsonStr);
  uint8_t dataArray[jsonStr.length()];
  jsonStr.toCharArray(dataArray, jsonStr.length() + 1);
  rf95.send(dataArray, sizeof(dataArray));  
  
}
