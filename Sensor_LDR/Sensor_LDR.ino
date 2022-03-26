#include <Adafruit_Sensor.h>
#define LDRPIN A0


void setup() {
  Serial.begin(9600);

}

void loop() {
  
  unsigned int AnalogValue;
  AnalogValue = analogRead(LDRPIN);
  Serial.println(AnalogValue); 
  delay(1000);
  

}
