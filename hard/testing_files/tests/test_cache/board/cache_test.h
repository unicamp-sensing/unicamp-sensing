
// TESTING HOW MANY DATAPOINTS CAN BE CACHED
//////////////
// The test consists in bombarding the board with the structs
// utilized to store the information from a location. 
// For a more acurate estimation, the original code of the
// boar is replicated in this test, since such code occupies
// memory space and should be taken in consideration. 
//Note that the test cannot define the remaining memory, 
// so in case of a stack overflow, the board crashes and restarts.
// APPROVAL:
//  - It will be succesfull if X datapoint can be stored
// FAILURE:  
//  - The Fail signal is never sent, must be time terminate by script



#define GOAL 600 // Amount of stored data required for success

int cache_overflow(std::queue<Data> dq, GPS& gps, DSM501a& prt, DHTwrap& dht) {
  // Insert data into the queue until the board crashes
  // Upon counting the ammount of nodes that can be inserted
  // in the list we can estimate the maximum cache and, knowing
  // the data collection frequency, estimate the time for caching
  int count = 0;
  while (true) {
    Serial.print("Inserting: ");
    Serial.println(++count);
    Data new_data(gps, prt, dht);
    dq.push(new_data);
    if (count >= GOAL) {
        Serial.println("The board succeded intcaching up to 600 datapoints");
        Serial.println("as promised in the repository's documentation.");
        Serial.println("");
        Serial.println("Pass");
        delay(10000);
    }
  }
  return 0;
}
