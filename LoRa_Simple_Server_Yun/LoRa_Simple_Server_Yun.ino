
#include <SPI.h>
#include <RH_RF95.h>
#include <PubSubClient.h>
#include <BridgeClient.h>
#include <Process.h>
#include <Console.h>


#define BAUDRATE 115200

BridgeClient net;
PubSubClient mqtt(net);


RH_RF95 rf95;

int led = A2;
float frequency = 868.0;
String mqttBroker = "test.mosquitto.org";

void setup_connection();
void connect_mqtt();

void setup() 
{
  pinMode(led, OUTPUT);     
  Bridge.begin(BAUDRATE);
  Console.begin();
  while (!Console) ; // Wait for console port to be available
  Console.println("Start Sketch");
  setup_connection();
  mqtt.setServer(mqttBroker. c_str(), 1883);
  
  if (!rf95.init())
    Console.println("init failed");
  
  rf95.setFrequency(frequency);
  
  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);
  Console.print("Listening on frequency: ");
  Console.println(frequency);
}

void loop()
{
  if (rf95.available())
  {

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      connect_mqtt();
      digitalWrite(led, HIGH);
      RH_RF95::printBuffer("request: ", buf, len);
      Console.print("got request: ");
      Console.println((char*)buf);
      char* data_gateway = (char*)buf;
      mqtt.publish("esp32/temphum", data_gateway);   
    }
    else
    {
      Console.println("recv failed");
    }
  }
  
//    Console.println("Asu");
//    delay(1000);
  mqtt.loop();
}

void setup_connection() {
   delay(10);
   Bridge.begin(BAUDRATE);
   randomSeed(micros());
}


void connect_mqtt()
{
    while (!mqtt.connected())
    {
        Serial.println("Connecting MQTT...");
        if (mqtt.connect("arduinoClient"))
        {
            Serial.println("Connected MQTT");
            mqtt.subscribe("esp32/temphum");
        }
    }
}
