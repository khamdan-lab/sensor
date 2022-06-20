
#include <SPI.h>
#include <RH_RF95.h>
#include <PubSubClient.h>
#include <BridgeClient.h>
#include <Process.h>
#include <Console.h>

#define BAUDRATE 115200

BridgeClient net;
PubSubClient mqtt(net);
bool isSubcribed = false;

RH_RF95 rf95;
char msg;
int led = A2;
float frequency = 868.0;
String mqttBroker = "test.mosquitto.org";

void setup_connection();
void connect_mqtt();

void callback(char* topic, byte* payload, unsigned int length) {
  Console.print("Setup Device :");
  for (int i=0;i<length;i++) {
    msg = (char)payload[i];
    String myString = String(msg); 
    uint8_t dataArray[myString.length()];
    myString.toCharArray(dataArray, myString.length() + 1);
//    Console.println((char *)dataArray);
    rf95.send(dataArray, sizeof(dataArray));
  }
  Console.println();
}

void setup() 
{
  pinMode(led, OUTPUT);     
  Bridge.begin(BAUDRATE);
  Console.begin();
  while (!Console) ; 
  Console.println("Start Sketch");
  setup_connection();
  mqtt.setServer(mqttBroker. c_str(), 1883);
  mqtt.setCallback(callback);
 
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
  connect_mqtt();
  if (rf95.available())
  {

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      connect_mqtt();
      digitalWrite(led, HIGH);
      RH_RF95::printBuffer("request: ", buf, len-1);
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
        Console.println("Connecting MQTT...");
        mqtt.subscribe("setupDevice");
        if (mqtt.connect("arduinoClient"))
        {
            Console.println("Connected MQTT");
            mqtt.subscribe("setupDevice");
        }
    }
}
