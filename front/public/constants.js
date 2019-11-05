const valueKeys = ["tmp", "hum", "pm10", "pm25"];


const valueKeyToRange = {
    tmp:  {min: 0, max: 40}, // temperature
    hum:  {min: 0, max: 150}, // humidity
    pm10: {min: 0, max: 200}, // PM 10
    pm25: {min: 0, max: 250}  // PM 2.5
};

const valueToColor = {
    tmp:  { scale: "Bluered", reverse: false },
    hum:  { scale: "YlGnBu",  reverse: true },
    pm10: { scale: "YlOrRd",  reverse: true },
    pm25: { scale: "Hot",     reverse: true },
};


const valueKeyToLayer = { tmp: "Temperature", hum: "Humidity", pm10: "PM10", pm25: "PM2.5" };

const valueKeyToUnit = { tmp: "°C", hum: "%RH", pm10: "", pm25: "" };
