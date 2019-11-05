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
|   |   |   |   |   |   |   +-- lat: [latitude]
|   |   |   |   |   |   |   +-- lon: [longitude]
|   |   |   |   |   |   |   +-- pm10: [PM10]
|   |   |   |   |   |   |   +-- pm25: [PM2.5]
```
## Running the website locally
Once you have [Node.js](https://nodejs.org/en/) installed and the repository cloned:
- `$ cd path/to/urban-sensing/front/`
- `$ npm install`
- `$ npm start`
- Open `http://localhost:PORT/`, where `PORT` will be printed at the terminal after running the command above (`3000` by default)
## Uploading Code To NodeMcu ESP-12
The code which controls the NodeMcu board and the sensors is located at master/hard/. Each sensor has a header file which implements wrappers and abstractions, these aim to create a simple, modular and asynchronous code. The main file (*board.ino*) contains the asynchronous calls for each sensor header, and a header of it's own with several declarations related to the board's pins and sensors parameters setup.

In order to compile the *board.ino* file on the Arduino IDE, you must first install the following libraries, which will either be installed via the "tools/manage libraries" option or by the "sketch/include library/add .zip library", by downloading from the github repo:
- adafruit unified sensor (1.0.3) - by Adafruit
- DHT sensor library (1.3.7) - by Adafruit
- ArduinoJson (5.13.1) - by Benoit Blanchon
- FirebaseExtended - https://github.com/FirebaseExtended/firebase-arduino/releases
*Note:* the versions of the libraries are relevant and might interfere in the code's compilation if not the same as the description.

With the libraries installed, ensure the headers and the main file *board.ino* are contained in the same folder. Within Arduino's IDE, select the option "Open Sketch" and choose the *board.ino* file. If the previous constraints are obeyed, the code can be loaded.

*Observations:*
- The DSM501a particle sensor can take up to a minute to warm up it's components.
- The GPS can take an indefinite time span to acquire satellite signal depending on the region it's located (at a open eviroment with good reception it can take up to 30 seconds).
- The data collected only has meanning if binded to it's Global Positioning, therefore, if there's no GPS signal, there's no data.
- Despite not necessary for collecting data, WiFi is required for sending it to the database. If there's no WiFi signal, the data will be cached and set when signal is acquired (data will be lost if there's no connection for too long). 
 
## Uploading changes to Heroku
Clone our repository and type the following commands:
- `$ cd path/to/urban-sensing/`
- `$ heroku git:remote -a unicamp-sensing`
- `$ git push heroku master`

Refer to [[link]](https://devcenter.heroku.com/articles/heroku-cli) for Heroku installation.

