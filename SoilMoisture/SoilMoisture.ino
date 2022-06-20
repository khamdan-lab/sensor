int sensorPin = A5;
int sensorValue = 0;
int humidity = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(sensorPin);
  humidity = convertToPercent(sensorValue);
  Serial.print("Nilai Sensor : ");
  Serial.println(sensorValue);
  Serial.print("Kelembaban: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.println(" ");
  delay(1000);
}

int convertToPercent(int value)
{
  int percentValue = 0;
  percentValue = map(value, 1023, 465, 0, 100);
  return percentValue;
}
