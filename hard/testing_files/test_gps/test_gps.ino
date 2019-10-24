#include <SoftwareSerial.h>
#include <stdlib.h>
#include <stdio.h>  
#include <time.h>
#include "gps.h"

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

  if (gps.newDop) {
    Serial.print("PDOP: ");
    Serial.println(gps.pdop, 2);
    Serial.print("HDOP & VDOP: ");
    Serial.print(gps.hdop, 2);
    Serial.print(" | ");
    Serial.print(gps.vdop, 2);
    Serial.println();
  }

  if (gps.newVel) {
    Serial.print("VEL: ");
    Serial.print(gps.vel);
    Serial.println();
  }
  
}

int timer = 5000;
int wait = (rand()%15)*1000;
int base = 0;
bool flag = false;

SoftwareSerial ss(4, 5);
GPS gps(ss, 9600); 

void setup()
{
  srand(time(NULL));
  Serial.begin(115200);
  delay(100);
}

void loop()
{

//  if (ss.available())
//    Serial.write(ss.read());
//  return;
  
  if (timer >= wait) {
    wait = (10+rand()%10)*1000;
    Serial.print("--> Wait time: ");
    Serial.println(wait/1000);
    base = millis();
    flag = !flag;
    timer = 0;
  } else {
    timer = millis() - base;
  }

  if (flag) gps.update();
  if (flag) test(gps);
}
