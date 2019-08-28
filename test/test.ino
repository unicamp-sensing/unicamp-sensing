// Required Libraries:
//    adafruit unified sensor (1.0.3) - by Adafruit
//    DHT sensor library (1.3.7) - by Adafruit
//    ArduinoJson (5.13.1) - by Benoit Blanchon
//    FirebaseExtended - https://github.com/FirebaseExtended/firebase-arduino/releases

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
#define WIFI_SSID "Shimbalaie_2.4GHz"
#define WIFI_PASSWORD "pipolina"

String myString;
int vr = A0; // variable resistor connected 
int sdata = 0; // The variable resistor value will be stored in sdata.

// Function which reads and sends the data
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Discard flawed reads
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Firebase.pushFloat("hum", h);
  Firebase.pushFloat("temp", t);
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
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop()
{
  sendSensor();
  delay(5000);            
}
