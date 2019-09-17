// Required Libraries:
//    adafruit unified sensor (1.0.3) - by Adafruit
//    DHT sensor library (1.3.7) - by Adafruit
//    ArduinoJson (5.13.1) - by Benoit Blanchon
//    FirebaseExtended - https://github.com/FirebaseExtended/firebase-arduino/releases

#include <stdio.h>

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <time.h>

// Specify DHT signal pin and model 
#define DHTPIN 0          // D3
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Set these to run example.
#define FIREBASE_HOST "teste-bb0d8.firebaseio.com"
#define FIREBASE_AUTH "mLOkguUxVGWlbYmAWdUYiHaYqWqDF9wHstkUIfTT"
#define WIFI_SSID "iPhone de vinicius"
#define WIFI_PASSWORD "shimbalaie"
#define TIMEZONE -3*3600
#define DST 0

// Mac address of device to use as id in the database
char mac[13];

char* readTime(){
  char buf[200];
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
//  Serial.print(p_tm->tm_mday);
//  Serial.print("/");
//  Serial.print(p_tm->tm_mon + 1);
//  Serial.print("/");
//  Serial.print(p_tm->tm_year + 1900);
//  
//  Serial.print(" ");
//  
//  Serial.print(p_tm->tm_hour);
//  Serial.print(":");
//  Serial.print(p_tm->tm_min);
//  Serial.print(":");
//  Serial.println(p_tm->tm_sec);
  sprintf(buf, "%.2d-%.2d-%.4d--%.2d-%.2d-%.2d", p_tm->tm_mday, p_tm->tm_mon + 1, p_tm->tm_year + 1900, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
  Serial.println(buf);
  return buf;
  delay(1000);
}

// Function which reads and sends the data
void sendSensor()
{
  static int count = 0; // Hash test
  char path[100];
  
  // Read sensor data
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  char timestr[200];
  strcpy(timestr, readTime());
  
  // Check and send humidity read
  if (!isnan(h)) {
    sprintf(path, "hum/%s/%s", mac, timestr);
    Firebase.setFloat(path,h);
  }
  
  // Check and send temperature data
  if (!isnan(t)) {
    sprintf(path, "tmp/%s/%s", mac, timestr);
    Firebase.setFloat(path,t);
  }

  // Hash test
  ++count;
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  //------- WiFi startup ----------//
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  //------- MAC address acquisition----------//
  byte m[6];
  WiFi.macAddress(m);
  sprintf(mac, "%X%X%X%X%X%X\0", m[0],m[1],m[2],m[3],m[4],m[5]);
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //-------time acquisition begin----------//
  configTime(TIMEZONE, DST, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for Internet time");

  while(!time(nullptr)){
     Serial.print("*");
     delay(1000);
  }
  Serial.println("\nTime response....OK");
  //-------time acquisition end----------//
}

void loop()
{
  sendSensor();
  delay(5000);            
}
