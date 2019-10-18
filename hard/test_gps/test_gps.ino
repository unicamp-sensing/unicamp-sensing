#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <string.h>

#define DEBUG 1
#define TZ -3
#define CENTURY 21
SoftwareSerial ss(4, 5) ; // LIGAR RX EM D1 E TX EM D2

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
  delay(100);
}

char* strcpyn(char* aux, char* str, int n) {
  for (int i = 0; i < n; ++i) aux[i] = str[i];
  aux[n] = '\0';
  return aux;
}

struct gps {
  // Time related
  bool newTime;
  int sec;
  int hour;
  int min;
  // Date related
  bool newDate;
  int day;
  int month;
  int year;
  // Location related
  bool newLocal;
  float lat;
  float lng;
  float alt;
  // Miscellaneous
  bool newVel;
  float vel;
} gps;


void gga_parsing(char *sentence) {
  char delim[] = ",";
  char aux[5], *ptr;

  strtok(sentence, delim); // Discard sentence ID

  // Time
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.hour = TZ + (int)strtol(strcpyn(aux, ptr, 2), NULL, 10);
    gps.min = (int)strtol(strcpyn(aux, &ptr[2], 2), NULL, 10);
    gps.sec = (int)strtol(strcpyn(aux, &ptr[4], 2), NULL, 10);
    gps.newTime = true;
  } else gps.newTime = false;

  // Latitude
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.lat = (float)strtof(strcpyn(aux, ptr, 2), NULL);
    gps.lat += (float)strtof(&ptr[2], NULL) / 60;
    // Adjust hemisphere
    ptr = strtok(NULL, delim);
    if (ptr[0] == 'S') gps.lat *= -1;
    gps.newLocal = true;
  } else gps.newLocal = false;

  // Longitude
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.lng = (float)strtof(strcpyn(aux, ptr, 3), NULL);
    gps.lng += (float)strtof(&ptr[3], NULL) / 60;
    // Adjust hemisphere
    ptr = strtok(NULL, delim);
    if (ptr[0] == 'W') gps.lng *= -1;
    gps.newLocal = true;
  } else gps.newLocal = false;

  strtok(NULL, delim); // Discard fix quality
  strtok(NULL, delim); // Discard tracked satellites
  strtok(NULL, delim); // Discard HDoP

  // Altitude
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.alt = strtof(ptr, NULL);
    // Check if in meters
    ptr = strtok(NULL, delim);
    if (ptr[0] != 'M') gps.alt = -1.1111;
    gps.newLocal = true;
  } else gps.newLocal = false;

  // Done parsing $GNGGA
  return;
}


void read_sentence(char *sentence) {
  int i = 0, stage = 1;
  char nmea[100];

  // Dump old serial input
  while (ss.available()) ss.read();

  // Parse new serial input
  while (true) {
    switch (stage) {
      
      case 1: // Finding the GGA sentence's starting point
        
        // Checks if theres data from the gps serial
        if (!ss.available()) continue;

        // Adjust sentence ID to first 6 chars
        for (i = 0; i < 5; ++i) nmea[i] = nmea[i + 1];  
        nmea[5] = ss.read();

        // Check it its a match and setup for stage 2
        if (!strncmp(nmea, sentence, 6)) {
          if (DEBUG) Serial.println("Got starting point!");
          stage = 2; 
          i = 5;
        }
        
      break;

      case 2: // Reading the GGA sentence to the buffer
        char curr;

        // Reads and adds next char to the array (blanks if no value)
        if (ss.available()) {
          curr = ss.read(); // read
          if (curr==',' && nmea[i]==',') nmea[++i] = ' '; // blank
          nmea[++i] = curr; // add
        }

        // Checks if reached the end of the sentence
        if (nmea[i] == '*') {
          nmea[i] = '\0';
          stage = 3;
          if (DEBUG) Serial.println("Read sentence:");
          if (DEBUG) Serial.println(nmea);
        }
        
      break;

      case 3: // Retrieving and casting info from the sentence
        if (!strcmp("$GNGGA", sentence))
          gga_parsing(nmea);
        else if (!strcmp("$GNRMC", sentence))
          rmc_parsing(nmea);
      
        return;
    }
  }
  
}

void rmc_parsing(char *sentence) {
  char delim[] = ",";
  char aux[5], *ptr;

  strtok(sentence, delim); // Discard sentence ID
  strtok(NULL, delim); // Discard time
  strtok(NULL, delim); // Discard status
  strtok(NULL, delim); // Discard latitude
  strtok(NULL, delim); // Discard hemisphere
  strtok(NULL, delim); // Discard longitude
  strtok(NULL, delim); // Discard hemisphere

  // Velocity
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.vel = strtof(ptr, NULL) / 1.944;
    gps.newVel = true;
  } else gps.newVel = true;

  strtok(NULL, delim); // Discard track angle

  // Date
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.day = (int)strtol(strcpyn(aux, ptr, 2), NULL, 10);
    gps.month = (int)strtol(strcpyn(aux, &ptr[2], 2), NULL, 10);
    gps.year = (int)(CENTURY - 1) * 100;
    gps.year += (int)strtol(strcpyn(aux, &ptr[4], 2), NULL, 10);
    gps.newDate = true;
  } else gps.newDate = true;

  strtok(NULL, delim); // Discard magnetic variation

  return;
}

void test(void) {

  Serial.print("DATE: ");
  Serial.print(gps.day);
  Serial.print("/");
  Serial.print(gps.month);
  Serial.print("/");
  Serial.print(gps.year);
  Serial.println();
  
  Serial.print("TIME: ");
  Serial.print(gps.hour);
  Serial.print(":");
  Serial.print(gps.min);
  Serial.print(":");
  Serial.print(gps.sec);
  Serial.println();
  
  Serial.println("LOCATION: ");
  Serial.print(gps.lat, 6);
  Serial.print(" | ");
  Serial.print(gps.lng, 6);
  Serial.println();

  Serial.println("ALT & VEL: ");
  Serial.print(gps.alt);
  Serial.print(" | ");
  Serial.print(gps.vel);
  Serial.println();
  
}

int timer = 5000;
int base = 0;
bool flag = false;

void loop()
{
  if (timer >= 5000) {
    while (ss.available()) ss.read();
    base = millis();
    flag = !flag;
    timer = 0;
  } else {
    timer = millis() - base;
  }

  if (flag) read_sentence("$GNGGA");
  if (flag) read_sentence("$GNRMC");
  if (flag) test();

}
