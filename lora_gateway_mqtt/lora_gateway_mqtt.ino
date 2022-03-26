  
#include <Console.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <BridgeClient.h>
#include <Process.h>
#include <RH_RF95.h>

#define BAUDRATE 115200

RH_RF95 rf9;
int led = A2;
float frequency = 868.0;
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* outTopic    = "esp32/temphum";

BridgeClient net;
PubSubClient client(net);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_connection() {
  
  Bridge.begin(BAUDRATE);
  randomSeed(micros());
  
}

void setup() {
  
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
  client.setServer(mqtt_server, 1883);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void gateway() {
  
  if (rf95.available()) {
    
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len)) {
        RH_RF95::printBuffer("request: ", buf, len);
        Console.print("got request: ");
        Console.println((char*)buf);
        Console.print("RSSI: ");
        Console.println(rf95.lastRssi(), DEC);
        
//        Send a reply
//        uint8_t data[] = "And hello back to you";
//        rf95.send(data, sizeof(data));
//        rf95.waitPacketSent();
//        Console.println("Sent a reply");
//        digitalWrite(led, LOW);
    }
    else {
      Console.println("recv failed");
    }
  }
}
