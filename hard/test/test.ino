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
#include <queue>

// Specify DHT signal pin and model 
#define DHTPIN 0          // D3
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Set these to run example.
#define FIREBASE_HOST "teste-bb0d8.firebaseio.com"
#define FIREBASE_AUTH "mLOkguUxVGWlbYmAWdUYiHaYqWqDF9wHstkUIfTT"
#define WIFI_SSID "AndroidAP"
#define WIFI_PASSWORD "rzqm8226"
#define TIMEZONE -3*3600
#define DST 0

// Mac address of device to use as id in the database
char mac[13];

typedef struct data {
  char timestr[200];
  float tmp;
  float hum;
} data;

std::queue<data> data_queue;

char* readTime(){
  char buf[200];
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  sprintf(buf, "%.4d/%.2d/%.2d/%.2d/%.2d:%.2d", p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
  Serial.println(buf);
  return buf;
  delay(1000);
}

// Function which reads and sends the data
void sendSensor()
{
  static int count = 0; // Hash test
  char path[100];

  data new_data;
  //get time string
  strcpy(new_data.timestr, readTime());
  
  // Read sensor data
  new_data.hum = dht.readHumidity();
  new_data.tmp = dht.readTemperature();

  //add data to queue
  data_queue.push(new_data);

  if (WiFi.status() == WL_CONNECTED) {
    while (!data_queue.empty()) {
      // Check and send humidity read
      data cur = data_queue.front();
      if (!isnan(cur.hum)) {
        sprintf(path, "%s/%s/hum", cur.timestr, mac);
        Firebase.setFloat(path, cur.hum);
      }
      
      // Check and send temperature data
      if (!isnan(cur.tmp)) {
        sprintf(path, "%s/%s/tmp", cur.timestr, mac);
        Firebase.setFloat(path, cur.tmp);
      }
      data_queue.pop();   
    }
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  // Connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
//  while (WiFi.status() != WL_CONNECTED)
//  {
//    Serial.print("wifi not connected\n");
//    delay(500);
//  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  // Getting mac address
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
