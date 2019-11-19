#include "DHTwrapper.h"
#define DMODEL DHT11

DHT aux(0,DMODEL);
DHTwrap dht(aux);

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  dht.update();
  Serial.print("HUM: ");
  Serial.println(dht.hum);
  Serial.print("TMP: ");
  Serial.println(dht.tmp);

  bool test = !isnan(dht.tmp) && !isnan(dht.hum);
  if (test) Serial.println("Pass");
  else Serial.println("Fail");

  delay(1000);
}
