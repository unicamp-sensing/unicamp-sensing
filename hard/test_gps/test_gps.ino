#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

#define TZ -3

TinyGPSPlus gps;          // The TinyGPS++ object
SoftwareSerial ss(4, 5) ; // LIGAR RX EM D1 E TX EM D2

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);       
}

void loop()
{
  bool valid =  gps.location.isValid()*gps.altitude.isValid();
  valid *= gps.time.isValid()*gps.hdop.isValid()*gps.speed.isValid();
  
  // Read all chars from serial communicatios  
  while (ss.available() > 0)
    if ( gps.encode( ss.read() )) //Ensures parsing

  if (gps.location.isUpdated() && valid)
  {
    Serial.println("------( Location Data )------");
    Serial.print("LAT: ");
    Serial.println(gps.location.lat());
    Serial.print("LON: ");
    Serial.println(gps.location.lng());
    Serial.println("------( Altitude Data )------");
    Serial.print(gps.altitude.meters());
    Serial.println("m");
    Serial.println("------( Speed Data )------");
    Serial.println(gps.speed.mps()); // Speed in meters per second (double)
    Serial.println("m/s");
    Serial.println("------( Time Data )------");
    int y = gps.date.year(); // Year (2000+) (u16)
    int mo = gps.date.month(); // Month (1-12) (u8)
    int d = gps.date.day(); // Day (1-31) (u8)
    Serial.println(y*10000+mo*100+d);
    Serial.print(gps.time.hour()+TZ);
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.print(gps.time.second());
    Serial.println();
    Serial.println("------( Satelites & Confidence )------");
    Serial.print("SATS: ");
    Serial.println(gps.satellites.value()); // Number of satellites in use (u32)
    Serial.print("HDOP: ");
    Serial.println((float)gps.hdop.value()/100); // Horizontal Dim. of Precision (100ths-i32)
  }

}
