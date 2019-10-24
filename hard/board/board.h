// Required Libraries:
//    adafruit unified sensor (1.0.3) - by Adafruit
//    DHT sensor library (1.3.7) - by Adafruit
//    ArduinoJson (5.13.1) - by Benoit Blanchon
//    FirebaseExtended - https://github.com/FirebaseExtended/firebase-arduino/releases

#include <stdio.h>
#include <queue>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include "DHTwrapper.h"
#include "DSM501a.h"
#include "gps.h"

// Set debugging flag
#define SHOW 1 

// Delay between each data collection (ms)
#define DELAY 3000

// Firebase addres and key
#define FIREBASE_HOST "teste-bb0d8.firebaseio.com"
#define FIREBASE_AUTH "mLOkguUxVGWlbYmAWdUYiHaYqWqDF9wHstkUIfTT"

// WiFi Login info
#define WIFI_SSID "iPhone de vinicius"
#define WIFI_PASSWORD "shimbalaie"

// Sensors pins and settings
#define DSM_PM10_PIN  12        // DSM501 Pin 2 on D6
#define DSM_PM25_PIN  13        // DSM501 Pin 4 on D7
#define DSM_SAMPLE_TIME 1.5     // Defined second for sampling collection 
#define WARM_UP_TIME 10         // Define seconds for sensor warmup

#define GPS_RX_PIN 5  // GPS RX pin on D1
#define GPS_TX_PIN 4  // GPS TX pin on D2

#define DHTTYPE DHT11  // DHT11 model
#define DHTPIN 0       // DHT11 pin 2 on D3

// Struct to store data to be sent
class Data {
  public:
    char timestr[200];
    double lati; // Latitude
    double lon;  // Longitude
    double alt;  // Altitude
    double vel;  // Velocity
    float tmp;   // temperature
    float hum;   // humidity
    float pm10;  // particle concentration for < 1um
    float pm25;  // particle concentration for < 2.5um

    // Get time and parse it
    char* parseTime(GPS& gps){
        char buf[30];
        sprintf(buf, "%.4d/%.2d/%.2d/%.2d/%.2d/%.2d", 
                gps.year, gps.month, gps.day, 
                gps.hour, gps.min, gps.sec);
        return buf;
    }

    Data (GPS& gps, DSM501a& particle, DHTwrap& dht) {
      // Save sensor data to struct
      strcpy(timestr, parseTime(gps));
      lati = gps.lat;
      lon = gps.lng;
      alt = gps.alt;
      vel = gps.vel;
      hum = dht.hum;
      tmp = dht.tmp;
      pm10 = particle.pm10;
      pm25 = particle.pm25;
    }

};
