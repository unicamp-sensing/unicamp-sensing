#ifndef DSM501A_H
#define DSM501A_H

#include <SoftwareSerial.h>

class DSM501a {
  // Variables and constructor
  private: 
    int PM10_PIN;
    int PM25_PIN;

  public:
    float pm10;
    float pm25;

  public:
    unsigned long smptime; // Sample time in milliseconds

    DSM501a (int pin_pm10, int pin_pm25, int sample) {
      smptime = sample*1000;
      PM10_PIN = pin_pm10;
      PM25_PIN = pin_pm25;
    }

  // Methods
  private:
   
    float getPM(int pin, bool hourly) {
      int start = millis();
      long lowpulseoccupancy = 0;
      
      while (millis() - start <= smptime) 
        lowpulseoccupancy += pulseIn(pin, LOW);
    
      float ratio = lowpulseoccupancy / (smptime*10.0);
      float concentration = 1.1*pow(ratio,3)-3.8 *pow(ratio,2)+520*ratio+0.62;
      if ( smptime < 3600000 && hourly) concentration *= ( smptime / 3600000.0 ); 
      
      return concentration;
    }
  
  public:
 
    void begin (int warm_up) {
      pinMode(PM10_PIN, INPUT);
      pinMode(PM25_PIN, INPUT);
      for (int i=0; i<warm_up; ++i) delay(1000);
    }

    float updatePM10(bool hourly){
      pm10 = getPM(PM10_PIN, hourly);
      return pm10;
    }

    float updatePM25(bool hourly){
      pm25 = getPM(PM25_PIN, hourly);
      return pm25;
    }

    void update(bool hourly) {
      updatePM10(hourly);
      updatePM25(hourly);
    }
    
};

#endif
