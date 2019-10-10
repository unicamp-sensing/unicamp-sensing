// Required Libraries:
//    adafruit unified sensor (1.0.3) - by Adafruit
//    DHT sensor library (1.3.7) - by Adafruit
//    ArduinoJson (5.13.1) - by Benoit Blanchon
//    FirebaseExtended - https://github.com/FirebaseExtended/firebase-arduino/releases
//    TinyGPS++.h (???) - ???

#include <stdio.h>

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <TinyGPS++.h>
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

// GPS global settings
#define TZ -3
TinyGPSPlus gps;          // The TinyGPS++ object
SoftwareSerial ss(4, 5) ; // LIGAR RX EM D1 E TX EM D2

// Mac address of device to use as id in the database
char mac[13];

typedef struct data {
  char timestr[200];
  double lati;
  double lon;
  double alt;
  double vel;
  float tmp;
  float hum;
} data;

std::queue<data> data_queue;

char* readTime(){
  char buf[200];
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  // Get datetime from GPS
  int y = gps.date.year(); // Year (2000+) (u16)
  int m = gps.date.month(); // Month (1-12) (u8)
  int d = gps.date.day(); // Day (1-31) (u8)
  int h = gps.time.hour(); // Hour (0-23) (u8)
  int m = gps.time.minute(); // Minute (0-59) (u8)
  int s = gps.time.second(); // Second (0-59) (u8)

  sprintf(buf, "%.4d/%.2d/%.2d/%.2d/%.2d/%.2d", y, m, d, h, m, s);
  Serial.println(buf);
  return buf;
}

// Function which reads and sends the data
void sendSensor()
{
  char path[100];
  data new_data;
  
  //get time string
  strcpy(new_data.timestr, readTime());
  
  // Read sensor data
  new_data.lati = gps.location.lat();
  new_data.lon = gps.location.lng();
  new_data.alt = gps.altitude.meters();
  new_data.vel = gps.speed.mps();
  new_data.hum = dht.readHumidity();
  new_data.tmp = dht.readTemperature();

  //add data to queue
  data_queue.push(new_data);


  ///// SENDING THE DATA (IF CONNECTED) /////
  if (WiFi.status() == WL_CONNECTED) {
    while (!data_queue.empty()) {
      // Check and send humidity read
      data cur = data_queue.front();
      
      if (!isnan(cur.hum)) {
        Serial.print(cur.hum);
        Serial.print("|");
        sprintf(path, "%s/%s/hum", cur.timestr, mac);
        Firebase.setFloat(path, cur.hum);
      }
      
      // Check and send temperature data
      if (!isnan(cur.tmp)) {
        Serial.print(cur.tmp);
        Serial.print("\n");
        sprintf(path, "%s/%s/tmp", cur.timestr, mac);
        Firebase.setFloat(path, cur.tmp);
      }
      data_queue.pop();   
    }
  }

  return;
}

void wait_gps(void){
  bool req = false;

  while (!req && ss.available()) {
    //Reads GPS serial
    while (ss.available()) gps.encode(ss.read())

    // Get if required data is valid
    req = gps.time.isUpdated();
    req *= gps.location.isValid()*gps.altitude.isValid();
    req *= gps.time.isValid()*gps.speed.isValid();
  }

  return;
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  
  // Setup GPS
  ss.begin(9600);  

  // Setup  DHT
  dht.begin();

  // Connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  // Getting mac address
  byte m[6];
  WiFi.macAddress(m);
  sprintf(mac, "%X%X%X%X%X%X\0", m[0],m[1],m[2],m[3],m[4],m[5]);
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

//  //-------time acquisition begin----------//
//  configTime(TIMEZONE, DST, "pool.ntp.org", "time.nist.gov");
//  Serial.println("\nWaiting for Internet time");
//
//  while(!time(nullptr)){
//     Serial.print("*");
//     delay(1000);
//  }
//  Serial.println("\nTime response....OK");
//  //-------time acquisition end----------//
}


void loop()
{
  
  wait_gps();  
  sendSensor();
  delay(10000);            
}
