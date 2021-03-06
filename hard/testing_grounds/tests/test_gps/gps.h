#ifndef GPS_H
#define GPS_H

#include <SoftwareSerial.h>
#define DEBUG 1
#define OVERTIME 5000
#define TZ -3
#define CENTURY 21

void read_sentence(char *sentence);

class GPS {
  public:
    SoftwareSerial *serial;
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
    // Precision
    bool newDop;
    float pdop;
    float hdop;
    float vdop;

    // Default Constructor 
    GPS (SoftwareSerial& ss, int baud) {
      serial = &ss; // LIGAR RX EM D1 E TX EM D2
      serial->begin(baud);
      bool newTime = false;
      bool newDate = false;
      bool newLocal = false;
      bool newVel = false;
      bool newDop = false;
    }

    void read_sentence(char *sentence);

    void update() {
      read_sentence("GGA");
      read_sentence("RMC");
      read_sentence("GSA");
    }
    
};

char* strcpyn(char* aux, char* str, int n) {
  for (int i = 0; i < n; ++i) aux[i] = str[i];
  aux[n] = '\0';
  return aux;
}

void gga_parsing(char *sentence, GPS& gps) {
  char delim[] = ",";
  char aux[10], *ptr;

  strtok(sentence, delim); // Discard sentence ID

  // Time
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.hour = (24+TZ+(int)strtol(strcpyn(aux, ptr, 2), NULL, 10))%24; // Fix Time Zone
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
    if (ptr[0] == 'M') gps.newLocal = true;
    else gps.newLocal = false;
  } else gps.newLocal = false;

  // Done parsing $GNGGA
  return;
}

void rmc_parsing(char *sentence, GPS& gps) {
  char delim[] = ",";
  char aux[10], *ptr;
  int day_fix = 0;

  strtok(sentence, delim); // Discard sentence ID
  
  // Time
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    int hour = (int)strtol(strcpyn(aux, ptr, 2), NULL, 10);
    if (TZ+hour < 0) day_fix = -1;  // Subtract one day
    if (TZ+hour >= 24) day_fix = 1; // Add one day
  }

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
  } else gps.newVel = false;

  strtok(NULL, delim); // Discard track angle

  // Date
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.day = day_fix+(int)strtol(strcpyn(aux, ptr, 2), NULL, 10);
    gps.month = (int)strtol(strcpyn(aux, &ptr[2], 2), NULL, 10);
    gps.year = (int)(CENTURY - 1) * 100;
    gps.year += (int)strtol(strcpyn(aux, &ptr[4], 2), NULL, 10);
    gps.newDate = true;
  } else gps.newDate = false;

  strtok(NULL, delim); // Discard magnetic variation

  return;
}

void gsa_parsing(char *sentence, GPS& gps) {
  char delim[] = ",";
  char aux[10], *ptr;

  strtok(sentence, delim); // Discard sentence ID
  strtok(NULL, delim); // Discard auto selection
  strtok(NULL, delim); // Discard fix dimensions
  for (int i=0; i<12; i++) strtok(NULL, delim); // Discard PRNs

  // PDOP
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.pdop = strtof(ptr, NULL);
    gps.newDop = true;
  } else gps.newDop = false;

  // VDOP
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.hdop = strtof(ptr, NULL);
    gps.newDop = true;
  } else gps.newDop = false;

  // HDOP
  ptr = strtok(NULL, delim);
  if ( ptr[0] != ' ' ) {
    gps.vdop = strtof(ptr, NULL);
    gps.newDop = true;
  } else gps.newDop = false;

  return;
}

void GPS::read_sentence(char *sentence) {
  int gap = 6 - strlen(sentence); // Identifiers subset
  int i = 0, stage = 1;
  char all_read[1000];
  int counter = 0;
  char nmea[100];

  // Dump old serial input
  while (serial->available()) serial->read();
  if (DEBUG) {Serial.print("Getting "); Serial.println(sentence);}
          

  int base_time = millis();
  // Parse new serial input
  while (millis()- base_time < OVERTIME) {
    switch (stage) {
      
      case 1: // Finding the GGA sentence's starting point
        
        // Checks if theres data from the gps serial
        if (!serial->available()) continue;

        // Adjust sentence ID to first 6 chars
        for (i = 0; i < 5; ++i) {
          nmea[i] = nmea[i + 1];  
        }
        nmea[5] = serial->read();
        all_read[counter++] = nmea[5];

        // Check it its a match and setup for stage 2
        if (!strncmp(&nmea[gap], sentence, gap)) {
          if (DEBUG) Serial.println("Got starting point!");
          stage = 2; 
          i = 5;
        }
        
      break;

      case 2: // Reading the GGA sentence to the buffer
        char curr;

        // Reads and adds next char to the array (blanks if no value)
        if (serial->available()) {
          curr = serial->read(); // read
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

        if (strstr(sentence, "GGA"))
          gga_parsing(nmea, *this);
        else if (strstr(sentence, "RMC"))
          rmc_parsing(nmea, *this);
        else if (strstr(sentence, "GSA"))
          gsa_parsing(nmea, *this);
        return;

      break;
    }
  }

  // If time overflow, discard read
  if (DEBUG) Serial.println("GPS Overtime (Set False Data)");
  if (DEBUG) Serial.println("Serial Input Read:");
  if (DEBUG) Serial.println(all_read);
  if (DEBUG) Serial.println("--------------------------------------------");
  newTime = false;
  newDate = false;
  newLocal = false;
  newVel = false;
  newDop = false;
}

#endif
