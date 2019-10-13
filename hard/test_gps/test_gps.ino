#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

#define TZ -3
#define CPMS 80000L

TinyGPSPlus gps;          // The TinyGPS++ object
SoftwareSerial ss(4, 5) ; // LIGAR RX EM D1 E TX EM D2

void readGps(void){
  while (ss.available() > 0) gps.encode(ss.read());

  if (gps.satellites.isUpdated() && gps.hdop.isUpdated())
  {
    Serial.println("------( Satelites & Confidence )------");
    Serial.print("SATS: ");
    Serial.println(gps.satellites.value()); // Number of satellites in use (u32)
    Serial.print("HDOP: ");
    Serial.println((float)gps.hdop.value()/100); // Horizontal Dim. of Precision (100ths-i32)
  }
  
  if (gps.location.isUpdated()) {
    Serial.println("------( Location Data )------");
    Serial.print("LAT: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("LON: ");
    Serial.println(gps.location.lng(), 6);
  }

  if (gps.altitude.isUpdated()) {
    Serial.println("------( Altitude Data )------");
    Serial.print(gps.altitude.meters());
    Serial.println("m");
  }

  if (gps.speed.isUpdated()) {
    Serial.println("------( Speed Data )------");
    Serial.println(gps.speed.mps()); // Speed in meters per second (double)
    Serial.println("m/s");
  }

  if (gps.date.isUpdated()) {
    Serial.println("------( Date Data )------");
    int y = gps.date.year(); // Year (2000+) (u16)
    int mo = gps.date.month(); // Month (1-12) (u8)
    int d = gps.date.day(); // Day (1-31) (u8)
    Serial.println(y*10000+mo*100+d);
  }

  if (gps.time.isUpdated()) {
    Serial.println("------( Time Data )------");
    Serial.print(gps.time.hour()+TZ);
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.print(gps.time.second());
    Serial.println();
  }

  timer0_write(ESP.getCycleCount() + 160000L);   // Pre-load timer with a time value (1-second)
}

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
  delay(100);

  noInterrupts();                                // Switch off interrupts whilst they are set up
  timer0_isr_init();                             // Initialise Timer0
  timer0_attachInterrupt(readGps);               // Goto the ISR function below when an interrupt occurs
  timer0_write(ESP.getCycleCount() + 160000L);   // Pre-load timer with a time value (1-second)
  interrupts();          
}

void loop()
{
//  delay(50);
//  while (ss.available() > 0) gps.encode(ss.read());
//
//  if (gps.satellites.isUpdated() && gps.hdop.isUpdated())
//  {
//    Serial.println("------( Satelites & Confidence )------");
//    Serial.print("SATS: ");
//    Serial.println(gps.satellites.value()); // Number of satellites in use (u32)
//    Serial.print("HDOP: ");
//    Serial.println((float)gps.hdop.value()/100); // Horizontal Dim. of Precision (100ths-i32)
//  } else return;
//  
//  if (gps.location.isUpdated()) {
//    Serial.println("------( Location Data )------");
//    Serial.print("LAT: ");
//    Serial.println(gps.location.lat(), 6);
//    Serial.print("LON: ");
//    Serial.println(gps.location.lng(), 6);
//  }
//
//  if (gps.altitude.isUpdated()) {
//    Serial.println("------( Altitude Data )------");
//    Serial.print(gps.altitude.meters());
//    Serial.println("m");
//  }
//
//  if (gps.speed.isUpdated()) {
//    Serial.println("------( Speed Data )------");
//    Serial.println(gps.speed.mps()); // Speed in meters per second (double)
//    Serial.println("m/s");
//  }
//
//  if (gps.date.isUpdated()) {
//    Serial.println("------( Date Data )------");
//    int y = gps.date.year(); // Year (2000+) (u16)
//    int mo = gps.date.month(); // Month (1-12) (u8)
//    int d = gps.date.day(); // Day (1-31) (u8)
//    Serial.println(y*10000+mo*100+d);
//  }
//
//  if (gps.time.isUpdated()) {
//    Serial.println("------( Time Data )------");
//    Serial.print(gps.time.hour()+TZ);
//    Serial.print(":");
//    Serial.print(gps.time.minute());
//    Serial.print(":");
//    Serial.print(gps.time.second());
//    Serial.println();
//  }
}
