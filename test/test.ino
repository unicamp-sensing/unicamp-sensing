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

// Specify DHT signal pin and model 
#define DHTPIN 0          // D3
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Set these to run example.
#define FIREBASE_HOST "teste-bb0d8.firebaseio.com"
#define FIREBASE_AUTH "mLOkguUxVGWlbYmAWdUYiHaYqWqDF9wHstkUIfTT"
#define WIFI_SSID "iPhone de vinicius"
#define WIFI_PASSWORD "shimbalaie"

// Mac address of device to use as id in the database
char mac[13];

// Function which reads and sends the data
void sendSensor()
{
  static int count = 0; // Hash test
  char path[30];
  
  // Read sensor data
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Check and send humidity read
  if (!isnan(h)) {
    sprintf(path, "hum/%s/%d", mac, count);
    Firebase.setFloat(path,h);
  }
  
  // Check and send temperature data
  if (!isnan(t)) {
    sprintf(path, "tmp/%s/%d", mac, count);
    Firebase.setFloat(path,t);
  }

  // Hash test
  ++count;
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  // Connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  // Getting mac address
  byte m[6];
  WiFi.macAddress(m);
  sprintf(mac, "%X%X%X%X%X%X\0", m[0],m[1],m[2],m[3],m[4],m[5]);
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop()
{
  sendSensor();
  delay(5000);            
}
