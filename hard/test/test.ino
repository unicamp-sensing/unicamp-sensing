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
#include "DSM501a.h"
#include "gps.h"

// Delay between each data collection (ms)
#define DELAY 3000

// Firebase addres and key
#define FIREBASE_HOST "teste-bb0d8.firebaseio.com"
#define FIREBASE_AUTH "mLOkguUxVGWlbYmAWdUYiHaYqWqDF9wHstkUIfTT"

// WiFi Login info
#define WIFI_SSID "iPhone de vinicius"
#define WIFI_PASSWORD "shimbalaie"

// Sensors settings
#define DSM_PM10_PIN  12        // DSM501 Pin 2 on D6
#define DSM_PM25_PIN  13        // DSM501 Pin 4 on D7
#define DSM_SAMPLE_TIME 1.5     // Defined second for sampling collection 
#define WARM_UP_TIME 10         // Define seconds for sensor warmup

#define GPS_RX_PIN 4  // GPS RX pin on D2
#define GPS_TX_PIN 5  // GPS TX pin on D1

#define DHTTYPE DHT11  // DHT11 model
#define DHTPIN 0       // DHT11 pin 2 on D3

class DHTwrap {
  private:
    DHT *dht;
  
  public:
    float hum;
    float tmp;

    DHTwrap (int pin, int model) {
      DHT aux(pin, model);
      (*dht) = aux;
    }

    void begin(void) {
      dht->begin();
    }

    void update(){
      hum = dht->readHumidity();
      tmp = dht->readTemperature();
    }
};

// DSM501a setup
DSM501a particle(DSM_PM10_PIN, DSM_PM25_PIN, DSM_SAMPLE_TIME);

// GPS setup
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);
GPS gps(ss, 9600); 

// DHT11 setup
DHTwrap dht(DHTPIN, DHTTYPE);

// Mac address of device to use as id in the database
char mac[13];

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

    Data (GPS& gps, DSM501a& particle, DHTwrap& dht, char* time) {
      // Save sensor data to struct
      strcpy(timestr, time);
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

// Create data queue as a offline cache
std::queue<Data> data_queue;


// Get time and parse it
char* readTime(){
  char buf[30];
  
  // Get datetime from GPS
  int y = gps.year; 
  int mo = gps.month;
  int d = gps.day; 
  int h = gps.hour; 
  int mi = gps.min;
  int s = gps.sec;

  sprintf(buf, "%.4d/%.2d/%.2d/%.2d/%.2d/%.2d", y, mo, d, h, mi, s);
  Serial.println(buf);
  return buf;
}


void show(Data& cur){
  Serial.print("|PM25:");
  Serial.print(cur.pm25);
  Serial.print("|PM10:");
  Serial.print(cur.pm10);
  Serial.print("|TMP:");
  Serial.print(cur.tmp);
  Serial.print("|HUM:");
  Serial.print(cur.hum);
  Serial.print("|VEL:");
  Serial.print(cur.vel);
  Serial.print("|ALT:");
  Serial.print(cur.alt);
  Serial.print("|LON:");
  Serial.print(cur.lon);
  Serial.print("|LAT:");
  Serial.print(cur.lati);              
}

// Send value to firebase
void sendFirebase(char* timestr, char* name, float val){
  char path[100];
  sprintf(path, "%s/%s/%s", timestr, mac, name);
  Firebase.setFloat(path, val);
}


// Function which reads and sends the data
void runSensors()
{
  // Update data
  gps.update();
  particle.update(true);

  // Construct class data to be sent
  Data new_data(gps, particle, dht, readTime());
  
  //add data to queue
  data_queue.push(new_data);

  bool requirements = gps.newTime && gps.newLocal;
  requirements *= (WiFi.status() == WL_CONNECTED);

  if (requirements) {
    while (!data_queue.empty()) {
      Data cur = data_queue.front();

      sendFirebase(cur.timestr, "lat", cur.lati);
      sendFirebase(cur.timestr, "lon", cur.lon);
      sendFirebase(cur.timestr, "alt", cur.alt); 
      sendFirebase(cur.timestr, "vel", cur.vel);
      
      if (!isnan(cur.hum))
        sendFirebase(cur.timestr, "hum", cur.hum);
      
      if (!isnan(cur.tmp))
        sendFirebase(cur.timestr, "tmp", cur.tmp);

      if (!isnan(cur.pm10))
        sendFirebase(cur.timestr, "pm10", cur.pm10);

      if (!isnan(cur.pm25))
        sendFirebase(cur.timestr, "pm25", cur.pm25);
      
      data_queue.pop();   
    }
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  
  // Start  DHT
  dht.begin();

  // Start DSM501A
  particle.begin(WARM_UP_TIME);

  // Start firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  // Connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Getting mac address
  byte m[6];
  WiFi.macAddress(m);
  sprintf(mac, "%X%X%X%X%X%X\0", m[0],m[1],m[2],m[3],m[4],m[5]);
}

void loop()
{
  runSensors();
  delay(DELAY);         
}
