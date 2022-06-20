int sensorPin = A3;
float voltageMax = 2.0;
float voltageMin = .4;
float voltageConversionConstant = .004882814;
float sensorVoltage = 0;

double x = 0;
double y = 0;
double a = 0;
double b = 0;

float windSpeedMin = 0;
float windSpeedMax = 32;

int windSpeed = 0;
int prevWindSpeed = 0;

void setup() {
Serial.begin(9600);

}

void loop() {

  int sensorValue = analogRead(sensorPin);

  float voltage = sensorValue * (5.0 / 1023.0);

  sensorVoltage = sensorValue * voltageConversionConstant;

  if (sensorVoltage <= voltageMin){
   windSpeed = 0; 
  }else {
    windSpeed = ((sensorVoltage - voltageMin)* windSpeedMax / (voltageMax - voltageMin)) * 2.232694; 
    Serial.println(windSpeed);
  }

  x = windSpeed;
  if (x >= y){
    y = x;
  }else{
    y = y; 
  }

  a = sensorVoltage;
  if (a >= b){
    b = a;  
  }else{
    b = b;
  }

  if (windSpeed != prevWindSpeed) {
    Serial.println(windSpeed);
    Serial.println(sensorVoltage);
    prevWindSpeed = windSpeed;
  }

  Serial.print("Wind Speed :");
  Serial.print(" ");
  Serial.println(windSpeed);
  delay(500);
}
