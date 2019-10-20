// External Required Libraries:
//    adafruit unified sensor (1.0.3) - by Adafruit
//    DHT sensor library (1.3.7) - by Adafruit
//    ArduinoJson (5.13.1) - by Benoit Blanchon
//    FirebaseExtended - https://github.com/FirebaseExtended/firebase-arduino/releases
//
// Pins Settings:
//  DSM501a - Particles Sensor:
//    - Pin 2 on D6 (PM1.0)
//    - Pin 4 on D7 (PM2.5)
//  NEO-6M (GY-GPS6M) - GPS:
//    - Pin RX on D1
//    - Pin TX on D2
//  DHT11 - Temp and Hum Sensor:
//    - Pin 2 on D3


#include <stdio.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <time.h>
#include <queue>
#include <TinyGPS++.h>

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
