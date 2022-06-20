#define WINDSPEED A0

void setup() {
  Serial.begin(9600);

}

void loop() {
 Serial.print("Wind Speed =");
 Serial.print(wind_speed());
 Serial.println("m/s");
 Serial.print(speed_mph());
 Serial.println("mph");

 delay(300);

}

float wind_speed(){
  float sensorValue = analogRead(WINDSPEED);
  float voltage = (sensorValue / 1023) * 5;
  float wind_speed = mapfloat(voltage, 0.4, 2, 0, 32.4);
  if (isnan(wind_speed) ){
      float wind_speed = 0.00;
      return wind_speed;
   }else{
     return wind_speed;
   }
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float speed_mph(){
  float speed_mph = ((wind_speed() *3600)/1609.344);
  if (isnan(speed_mph) ){
      float speed_mph = 0.00;
      return speed_mph;
   }else{
     return speed_mph;
   }
}
