#include <SoftwareSerial.h>
#include "DSM501a.h"

DSM501a particle(12, 13, 2);

void setup()
{
  // Debug console
  Serial.begin(115200);
  particle.begin(3);
  Serial.println("Ready!");
}

void loop()
{
  int p25, p10;

  for (int i = 0; i < 5; ++i) {
    p10 = particle.getPM10(1);
    Serial.print("PM10: ");
    Serial.println(p10);
    p25 = particle.getPM25(1);
    Serial.print("PM25: ");
    Serial.println(particle.getPM25(1));
    
    if (p10 < 1 || p25 < 1) {
      Serial.println("Fail");
    }
    delay(1000);
  }
  
  Serial.println("Pass");
  delay(1000);
}
