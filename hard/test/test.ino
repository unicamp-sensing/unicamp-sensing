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
#include <DHT.h>
#include <time.h>
#include <queue>
#include <TinyGPS++.h>

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

// GPS global settings
#define TZ -3
TinyGPSPlus gps;          // The TinyGPS++ object
SoftwareSerial ss(4, 5) ; // LIGAR RX EM D1 E TX EM D2

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
  double lati;
  double lon;
  double alt;
  double vel;
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




char* readTime(){
  char buf[200];
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  // Get datetime from GPS
  int y = gps.date.year(); // Year (2000+) (u16)
  int mo = gps.date.month(); // Month (1-12) (u8)
  int d = gps.date.day(); // Day (1-31) (u8)
  int h = gps.time.hour()+TZ; // Hour (0-23) (u8)
  int mi = gps.time.minute(); // Minute (0-59) (u8)
  int s = gps.time.second(); // Second (0-59) (u8)

  Serial.println(y*10000+mo*100+d);
  sprintf(buf, "%.4d/%.2d/%.2d/%.2d/%.2d/%.2d", y, mo, d, h, mi, s);
  Serial.println(buf);
  return buf;
}


void read_gps(void){
  // Get if required data is valid
  bool req = false;

  //Reads GPS serial
  while (ss.available()) {
    gps.encode( ss.read() );
    req = gps.time.isUpdated();
    req *= gps.location.isValid()*gps.altitude.isValid();
    req *= gps.time.isValid()*gps.speed.isValid();
  }

  if (gps.time.isUpdated())
  {
    Serial.println("------( Location Data )------");
    Serial.print("LAT: ");
    Serial.println(gps.location.lat());
    Serial.print("LON: ");
    Serial.println(gps.location.lng());
    Serial.println("------( Altitude Data )------");
    Serial.print(gps.altitude.meters());
    Serial.println("m");
    Serial.println("------( Speed Data )------");
    Serial.println(gps.speed.mps()); // Speed in meters per second (double)
    Serial.println("m/s");
    Serial.println("------( Time Data )------");
    int y = gps.date.year(); // Year (2000+) (u16)
    int mo = gps.date.month(); // Month (1-12) (u8)
    int d = gps.date.day(); // Day (1-31) (u8)
    Serial.println(y*10000+mo*100+d);
    Serial.print(gps.time.hour()+TZ);
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.print(gps.time.second());
    Serial.println();
  }
  
  return;
}

///////////////////////////////////////////////////////////////////////////////////

void test() {
  bool valid =  gps.location.isValid()*gps.altitude.isValid();
  valid *= gps.time.isValid()*gps.hdop.isValid()*gps.speed.isValid();
  
  // Read all chars from serial communicatios  
  while (ss.available() > 0) gps.encode( ss.read() ); //Ensures parsing
    
  if (gps.location.isUpdated() && valid)
  {
    Serial.println("------( Location Data )------");
    Serial.print("LAT: ");
    Serial.println(gps.location.lat());
    Serial.print("LON: ");
    Serial.println(gps.location.lng());
    Serial.println("------( Altitude Data )------");
    Serial.print(gps.altitude.meters());
    Serial.println("m");
    Serial.println("------( Speed Data )------");
    Serial.println(gps.speed.mps()); // Speed in meters per second (double)
    Serial.println("m/s");
    Serial.println("------( Time Data )------");
    int y = gps.date.year(); // Year (2000+) (u16)
    int mo = gps.date.month(); // Month (1-12) (u8)
    int d = gps.date.day(); // Day (1-31) (u8)
    Serial.println(y*10000+mo*100+d);
    Serial.print(gps.time.hour()+TZ);
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.print(gps.time.second());
    Serial.println();
    Serial.println("------( Satelites & Confidence )------");
    Serial.print("SATS: ");
    Serial.println(gps.satellites.value()); // Number of satellites in use (u32)
    Serial.print("HDOP: ");
    Serial.println((float)gps.hdop.value()/100); // Horizontal Dim. of Precision (100ths-i32)
  }
}
////////////////////////////////////////////////////////////////////////////////////




void read_particle(data &new_data) {
  int starttime = millis();
  
  while (millis()- starttime < sampletime_ms) {
    AQI.lowpulseoccupancyPM10 += pulseIn(DUST_SENSOR_DIGITAL_PIN_PM10, LOW);
    AQI.lowpulseoccupancyPM25 += pulseIn(DUST_SENSOR_DIGITAL_PIN_PM25, LOW);
    while (ss.available() > 0) gps.encode( ss.read() );
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
}




// Function which reads and sends the data
void sendSensor()
{
  static int count = 0; // Hash test
  char path[100];

  data new_data;
  
  // Read sensor data
  read_particle(new_data); // 10s read delay
  read_gps(); // Get most recent data from GPS serial
  strcpy(new_data.timestr, readTime());
  
  new_data.lati = gps.location.lat();
  new_data.lon = gps.location.lng();
  new_data.alt = gps.altitude.meters();
  new_data.vel = gps.speed.mps();
  new_data.hum = dht.readHumidity();
  new_data.tmp = dht.readTemperature();

  //add data to queue
  data_queue.push(new_data);

  if (WiFi.status() == WL_CONNECTED) {
    while (!data_queue.empty()) {
      // Check and send humidity read
      data cur = data_queue.front();

      Serial.print(cur.lati);
      Serial.print("|LAT:");
      sprintf(path, "%s/%s/lat", cur.timestr, mac);
      Firebase.setFloat(path, cur.lati);
      
      Serial.print(cur.lon);
      Serial.print("|LON:");
      sprintf(path, "%s/%s/lon", cur.timestr, mac);
      Firebase.setFloat(path, cur.lon);

      Serial.print(cur.alt);
      Serial.print("|ALT:");
      sprintf(path, "%s/%s/alt", cur.timestr, mac);
      Firebase.setFloat(path, cur.alt); 

      Serial.print(cur.vel);
      Serial.print("|VEL:");
      sprintf(path, "%s/%s/alt", cur.timestr, mac);
      Firebase.setFloat(path, cur.vel);
      
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
  Serial.begin(115200);
  
  // Setup GPS
  ss.begin(9600);  

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
}

void loop()
{
  test();
  sendSensor();
//  delay(10000);            
}
