#include <SoftwareSerial.h>
#include "DSM501a.h"

DSM501a particle(12, 13, 2);

void setup()
{
  // Debug console
  Serial.begin(115200);
  particle.begin(0);
  Serial.println("Ready!");
}

void loop()
{
  Serial.print("PM10: ");
  Serial.println(particle.getPM10(1));
  Serial.print("PM25: ");
  Serial.println(particle.getPM25(1));
}
