#ifndef DHT_WRAPPER
#define DHT_WRAPPER

#include <DHT.h>

class DHTwrap {
  private:
    DHT *dht;
  
  public:
    float hum;
    float tmp;

    DHTwrap (DHT& aux) {
      dht = &aux;
    }

    void begin(void) {
      dht->begin();
    }

    void update(){
      hum = dht->readHumidity();
      tmp = dht->readTemperature();
    }
};

#endif
