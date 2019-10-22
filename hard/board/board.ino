#include "board.h"

// DSM501a setup
DSM501a particle(DSM_PM10_PIN, DSM_PM25_PIN, DSM_SAMPLE_TIME);
// GPS setup
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);
GPS gps(ss, 9600); 
// DHT11 setup
DHT aux(DHTPIN, DHTTYPE);
DHTwrap dht(aux);
// Create data queue as a offline cache
std::queue<Data> data_queue;
// Mac address of device to use as id in the database
char mac[13];

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
  particle.update(true);
  dht.update();
  gps.update();
  // Construct class data to be sent
  Data new_data(gps, particle, dht);
  //add data to queue
  data_queue.push(new_data);
}

void sendInfo() {
  // Define prerequisites to send data
  bool requirements = gps.newTime && gps.newLocal;
  requirements *= (WiFi.status() == WL_CONNECTED);

  // Send data
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
  sendInfo();
  delay(DELAY);         
}
