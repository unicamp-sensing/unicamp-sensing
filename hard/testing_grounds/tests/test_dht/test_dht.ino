#include "DHTwrapper.h"
#define DMODEL DHT11

DHT aux(0,DMODEL);
DHTwrap dht(aux);

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  dht.begin();
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  dht.update();

  for (int i = 0; i < 5; ++i) {
    Serial.print("HUM: ");
    Serial.println(dht.hum);
    Serial.print("TMP: ");
    Serial.println(dht.tmp);
    if (isnan(dht.tmp) || isnan(dht.hum)) {
      Serial.println("Fail");
    }
    delay(1000);
  }
  
  Serial.println("Pass");

  delay(1000);
}
