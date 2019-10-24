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

Refer to [[link]](https://devcenter.heroku.com/articles/heroku-cli) for Heroku installation.