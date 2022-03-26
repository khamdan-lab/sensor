#define PHTANAHPIN A1
int sensorValue = 0;        
float outputValue = 0.0; 

void setup() {
  Serial.begin(9600);
  
}

void loop() {
  
  sensorValue = analogRead(PHTANAHPIN);
  delay(1000);
  outputValue = (-0.0139*sensorValue)+7.7851;
  Serial.print("adc= ");
  Serial.println(sensorValue);
  Serial.print("ph= ");
  Serial.println(outputValue);

}
