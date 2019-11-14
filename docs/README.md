# Docs
## Firebase data format
```bash
+-- [year]
|   +-- [month]
|   |   +-- [day]
|   |   |   +-- [hour]
|   |   |   |   +-- [minute]
|   |   |   |   |   +-- [second]
|   |   |   |   |   |   +-- [board_MAC_addr]
|   |   |   |   |   |   |   +-- hum: [humidity]
|   |   |   |   |   |   |   +-- tmp: [temperature]
```
## Running the website locally
Once you have [Node.js](https://nodejs.org/en/) installed and the repo cloned:
- `$ cd path/to/urban-sensing/front/`
- `$ npm install`
- `$ node index.js`
- Open `http://localhost:PORT/`, where `PORT` will be printed at the terminal after running the command above
## Setting up the environment on Arduino IDE to load the code in the NodeMCU
In order to compile the *test.ino* file on the Arduino IDE, you must first install the following libraries, which will either be installed via the "tools/manage libraries" option or by the "sketch/include library/add .zip library", by downloading from the github repo:
- adafruit unified sensor (1.0.3) - by Adafruit
- DHT sensor library (1.3.7) - by Adafruit
- ArduinoJson (5.13.1) - by Benoit Blanchon
- FirebaseExtended - https://github.com/FirebaseExtended/firebase-arduino/releases
## Uploading to Heroku
Clone our repository and type the following commands:
- `$ cd path/to/urban-sensing/`
- `$ heroku git:remote -a unicamp-sensing`
- `$ git push heroku master`
## Hardware Information
# Configuration
- The data collection frequency can be delayed setting the constant at `hard/board/board.h`. The DELAY is idle time, but the DSM sampling time must be carefully set.
- The sensor can stack about 600 datapoints (depends on the queue's node size). Depending on the data collection frequency the amount of offline time which data can be preserved will vary (minimum of about 1 hour).
- When sending cached data, the "burst" of cached data can be defined at `hard/board/board.h`. Setting too high can cause the collection of data to halt due to the necessary time for uploading the data. Setting too low might cause data loss due to lack of space in the cache. 