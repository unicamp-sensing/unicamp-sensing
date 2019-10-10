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
#include <SimpleTimer.h>

/* Connect the DSM501 sensor as follow 
 * https://www.elektronik.ropla.eu/pdf/stock/smy/dsm501.pdf
 * 1 green vert - Not used
 * 2 yellow jaune - Vout2 - 1 microns (PM1.0)
 * 3 white blanc - Vcc
 * 4 red rouge - Vout1 - 2.5 microns (PM2.5)
 * 5 black noir - GND
*/
#define DUST_SENSOR_DIGITAL_PIN_PM10  12        // DSM501 Pin 2 on D6
#define DUST_SENSOR_DIGITAL_PIN_PM25  13        // DSM501 Pin 4 on D7 
//#define COUNTRY                       2         // 0. France, 1. Europe, 2. USA/China
//#define EXCELLENT                     "Excellent"
//#define GOOD                          "Good"
//#define ACCEPTABLE                    "Acceptable"
//#define MODERATE                      "Moderate"
//#define HEAVY                         "Heavy"
//#define SEVERE                        "Severe"
//#define HAZARDOUS                     "Hazardous"

unsigned long   duration;
unsigned long   starttime;
unsigned long   endtime;
unsigned long   lowpulseoccupancy = 0;
float           ratio = 0;
unsigned long   SLEEP_TIME    = 2 * 1000;       // Sleep time between reads (in milliseconds)
unsigned long   sampletime_ms = 10 * 1000;  // Durée de mesure - sample time (ms)

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
  float tmp; //temperature
  float hum; //humidity
  float pm10; //particle concentration for < 1um
  float pm25; ////particle concentration for < 2.5um
} data;

std::queue<data> data_queue;


struct structAQI{
  // variable enregistreur - recorder variables
  unsigned long   durationPM10;
  unsigned long   lowpulseoccupancyPM10 = 0;
  unsigned long   durationPM25;
  unsigned long   lowpulseoccupancyPM25 = 0;
  unsigned long   starttime;
  unsigned long   endtime;
  // Sensor AQI data
  float         concentrationPM25 = 0;
  float         concentrationPM10  = 0;
  int           AqiPM10            = -1;
  int           AqiPM25            = -1;
  // Indicateurs AQI - AQI display
  int           AQI                = 0;
  String        AqiString          = "";
  int           AqiColor           = 0;
};
struct structAQI AQI;

SimpleTimer timer;



char* readTime(){
  char buf[200];
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  sprintf(buf, "%.4d/%.2d/%.2d/%.2d/%.2d/%.2d", p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
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

  int starttime = millis();
  while (millis()- starttime < sampletime_ms) {
    AQI.lowpulseoccupancyPM10 += pulseIn(DUST_SENSOR_DIGITAL_PIN_PM10, LOW);
    AQI.lowpulseoccupancyPM25 += pulseIn(DUST_SENSOR_DIGITAL_PIN_PM25, LOW);
  }
  
  //update measurements
  //AQI.endtime = millis();
  float ratio = AQI.lowpulseoccupancyPM10 / (sampletime_ms * 10.0);
  float concentration = 1.1 * pow( ratio, 3) - 3.8 *pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve
  if ( sampletime_ms < 3600000 ) { concentration = concentration * ( sampletime_ms / 3600000.0 ); }
  AQI.lowpulseoccupancyPM10 = 0;
  AQI.concentrationPM10 = concentration;
  new_data.pm10 = concentration;
  
  ratio = AQI.lowpulseoccupancyPM25 / (sampletime_ms * 10.0);
  concentration = 1.1 * pow( ratio, 3) - 3.8 *pow(ratio, 2) + 520 * ratio + 0.62;
  if ( sampletime_ms < 3600000 ) { concentration = concentration * ( sampletime_ms / 3600000.0 ); }
  AQI.lowpulseoccupancyPM25 = 0;
  AQI.concentrationPM25 = concentration;
  new_data.pm25 = concentration;

  //Serial.print("Concentrations => PM2.5: "); Serial.print(AQI.concentrationPM25); Serial.print(" | PM10: "); Serial.println(AQI.concentrationPM10);
  
  //AQI.starttime = millis();
      
//  // Actualise l'AQI de chaque capteur - update AQI for each sensor 
//  if ( COUNTRY == 0 ) {
//    // France
//    AQI.AqiPM25 = getATMO( 0, AQI.concentrationPM25 );
//    AQI.AqiPM10 = getATMO( 1, AQI.concentrationPM10 );
//  } else if ( COUNTRY == 1 ) {
//    // Europe
//    AQI.AqiPM25 = getACQI( 0, AQI.concentrationPM25 );
//    AQI.AqiPM10 = getACQI( 1, AQI.concentrationPM10 );
//  } else {
//    // USA / China
//    AQI.AqiPM25 = getAQI( 0, AQI.concentrationPM25 );
//    AQI.AqiPM10 = getAQI( 0, AQI.concentrationPM10 );
//  }
//
//  // Actualise l'indice AQI - update AQI index
//  updateAQILevel();
//  updateAQIDisplay();
//  
//  Serial.print("AQIs => PM25: "); Serial.print(AQI.AqiPM25); Serial.print(" | PM10: "); Serial.println(AQI.AqiPM10);
//  Serial.print(" | AQI: "); Serial.println(AQI.AQI); Serial.print(" | Message: "); Serial.println(AQI.AqiString);

  

  //add data to queue
  data_queue.push(new_data);

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

      // Check and send temperature data
      if (!isnan(cur.pm10)) {
        Serial.print(cur.pm10);
        Serial.print("\n");
        sprintf(path, "%s/%s/pm10", cur.timestr, mac);
        Firebase.setFloat(path, cur.pm10);
      }

      // Check and send temperature data
      if (!isnan(cur.pm25)) {
        Serial.print(cur.pm25);
        Serial.print("\n");
        sprintf(path, "%s/%s/pm25", cur.timestr, mac);
        Firebase.setFloat(path, cur.pm25);
      }
      
      data_queue.pop();   
    }
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  // Setup  DHT
  dht.begin();

  //Setup DSM501A
  pinMode(DUST_SENSOR_DIGITAL_PIN_PM10,INPUT);
  pinMode(DUST_SENSOR_DIGITAL_PIN_PM25,INPUT);

  // wait 10s for DSM501 to warm up
  for (int i = 1; i <= 10; i++)
  {
    delay(1000); // 1s
    Serial.print(i);
    Serial.println(" s (wait 10s for DSM501 to warm up)");
  }
  
  Serial.println("Ready!");
  
  AQI.starttime = millis();
  //timer.setInterval(sampletime_ms, updateAQI);

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
  //delay(10000);            
}
