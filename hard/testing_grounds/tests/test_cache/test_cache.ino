#include "board.h"

// DSM501a setup
DSM501a particle(DSM_PM10_PIN, DSM_PM25_PIN, DSM_SAMPLE_TIME);
// GPS setup
SoftwareSerial ss(GPS_TX_PIN, GPS_RX_PIN);
GPS gps(ss, 9600); 
// DHT11 setup
DHT aux(DHTPIN, DHTTYPE);
DHTwrap dht(aux);
// Create data queue as a offline cache
std::queue<Data> data_queue;
// Mac address of device to use as id in the database
char mac[13];


// TEST FUNCTIONS ///////
#include "test_cache.h"//
/////////////////////////



// Send value to firebase
void sendFirebase(char* timestr, char* name, float val){
  char path[100];
  sprintf(path, "%s/%s/%s", timestr, mac, name);
  if (SHOW) {Serial.print(path); Serial.print(" | "); Serial.println(val);}
  Firebase.setFloat(path, val);
}


// Function which reads and sends the data
void runSensors()
{
  if (SHOW) Serial.println("---> runSensors()");
  
  // Update data
  if (SHOW) Serial.println("Updating:");
  particle.update(true);
  if (SHOW) Serial.println("DSM501A Done!");
  dht.update();
  if (SHOW) Serial.println("DHT11 Done!");
  gps.update();
  if (SHOW) Serial.println("GPS Done!");

  // Define requirements for colectting the data
  bool requirements = gps.newTime && gps.newLocal;
  
  if (requirements){
    if (SHOW) Serial.println("Stacking Data");
    // Construct class data to be sent
    Data new_data(gps, particle, dht);
    //add data to queue
    data_queue.push(new_data); 
  } 
  else if (SHOW) Serial.println("****No GPS Signal****");

  if (SHOW) Serial.println("<--- runSensors()");
}

void sendInfo() {
  if (SHOW) Serial.println("---> sendInfo()");

  // Send data
  if (WiFi.status() == WL_CONNECTED) {
    while (!data_queue.empty()) {
      if (SHOW) Serial.println("Sending Data:");
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
      if (SHOW) Serial.println("Sent");     
    }
  }
  else if (SHOW) Serial.println("****No WiFi Signal****"); 

  
  if (SHOW) Serial.println("<--- sendInfo()");
}

void setup()
{
  // Debug console
  if (SHOW) Serial.begin(115200);
  if (SHOW) Serial.println("\n---> Setup()");
  
  // Start  DHT
  dht.begin();
  if (SHOW) Serial.println("DHT Set");

  // Start DSM501A
  particle.begin(0);
  if (SHOW) Serial.println("DSM501a Set");

  // Start firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  if (SHOW) Serial.println("Firebase Set");
  
  // Connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (SHOW) Serial.println("WiFi Set");

  // Getting mac address
  byte m[6];
  WiFi.macAddress(m);
  sprintf(mac, "%X%X%X%X%X%X\0", m[0],m[1],m[2],m[3],m[4],m[5]);
  if (SHOW) Serial.println("Got MAC Address");
  
  if (SHOW) Serial.println("<--- setup()!");
}

void loop()
{
  if (SHOW) Serial.println("---> loop()");
  runSensors();
  sendInfo();

  // THE IMPORTANT PART: ///////
  // Call the testing function//
  cache_overflow(data_queue, gps, particle, dht);
  //////////////////////////////
  
  
  delay(DELAY);
  if (SHOW) Serial.println("<--- loop()");         
}
