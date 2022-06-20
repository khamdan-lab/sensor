
#define BAUDRATE 115200

#include <Console.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <PubSubClient.h>
#include <BridgeClient.h>
#include <Process.h>

RH_RF95 rf95;

BridgeClient net;
PubSubClient mqtt(net);

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
  if (!rf95.init())
    Console.println("init failed");
  rf95.setFrequency(frequency);

  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);

  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);

  Console.print("Listening on frequency: ");
  Console.println(frequency);

  setup_connection();
  mqtt.setServer(mqttBroker. c_str(), 1883);
}

void loop()
{
  if (!mqtt.connected())
  {
    connect_mqtt();
    Console.println("MQTT Connected");
    mqtt.publish("smartpaddy/polindra", "ESP 32 Online!");
  }
  mqtt.loop();
  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      digitalWrite(led, HIGH);
      RH_RF95::printBuffer("request: ", buf, len);
      Console.print("got request: ");
      Console.println((char*)buf);
      mqtt.publish("smartpaddy/polindra", (char*)buf);
    }
    else
    {
      Console.println("recv failed");
    }
  }
  delay(2000);
}


void setup_connection() {
  delay(10);
  Bridge.begin(BAUDRATE);
  randomSeed(micros());
  Console.println("Setup Connection");
}



void connect_mqtt() {
  while (!mqtt.connected()) {
    Console.println("Connecting MQTT...");
    if (mqtt.connect("esp32")) {
      mqtt.subscribe("smartpaddy/polindra");
    }
  }
}
