#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include "gpsLib.h"

void test(GPS& gps) {

  if (gps.newDate) {
    Serial.print("DATE: ");
    Serial.print(gps.day);
    Serial.print("/");
    Serial.print(gps.month);
    Serial.print("/");
    Serial.print(gps.year);
    Serial.println();
  }

  if (gps.newTime) {  
    Serial.print("TIME: ");
    Serial.print(gps.hour);
    Serial.print(":");
    Serial.print(gps.min);
    Serial.print(":");
    Serial.print(gps.sec);
    Serial.println();
  }

  if (gps.newLocal) {
    Serial.print("LOCATION: ");
    Serial.print(gps.lat, 6);
    Serial.print(" | ");
    Serial.print(gps.lng, 6);
    Serial.println();
    Serial.print("ALT: ");
    Serial.println(gps.alt);
  }

  if (gps.newVel) {
    Serial.print("VEL: ");
    Serial.print(gps.vel);
    Serial.println();
  }
  
}

int timer = 5000;
int base = 0;
bool flag = false;

SoftwareSerial ss(4, 5);
GPS gps(ss, 9600); 

void setup()
{
  Serial.begin(115200);
  delay(100);
}

void loop()
{
  if (timer >= 5000) {
    base = millis();
    flag = !flag;
    timer = 0;
  } else {
    timer = millis() - base;
  }

  if (flag) gps.read_sentence("$GNGGA");
  if (flag) gps.read_sentence("$GNRMC");
  if (flag) test(gps);
}
