#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

static const int RXPin = 5, TXPin = 6;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
float lat ,lon ;
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);

}

void loop()
{
  
  Serial.println(getLatitude());
 
}


String getLatitude(){
  while(ss.available()){
    if (gps.encode(ss.read())){  
      if (gps.location.isValid()) {
         lat = gps.location.lat();     
      }
    }
    String latitude = String(lat,6);
    return latitude;
  }
}

//float getLongitude(){
//  if (gps.encode(ss.read())){  
//    if (gps.location.isValid()) {
//      float longitude = gps.location.lng();
//      return longitude;
//      delay(5000);
//    }
//    else {
//      float longitude = 0.000000;
//      return longitude;
//    }
//    
//  }
//}
    
