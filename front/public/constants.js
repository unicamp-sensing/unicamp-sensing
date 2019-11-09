/**
 * General properties and constants we need for each of the values that are collected */

const Props = {
    tmp: {
        name: "Temperature",
        unit: "°C",
        range: { min: 0, max: 50 },
        plot: {
            time: {
                color: { reverse: false, scale: "Bluered" },
            },
            map: {
                layer: "Temperature",
            },
        }
    },
    hum: {
        name: "Humidity",
        unit: "%RH",
        range: { min: 0, max: 100 },
        plot: {
            time: {
                color: { reverse: true, scale: "YlGnBu" },
            },
            map: {
                layer: "Humidity",
            },
        }
    },
    pm10: {
        name: "PM10",
        unit: "", // FIXME
        range: { min: 0, max: 250 },
        plot: {
            time: {
                color: { reverse: true, scale: "YlOrRd" },
            },
            map: {
                layer: "PM10",
            },
        }
    },
    pm25: {
        name: "PM2.5",
        unit: "", // FIXME
        range: { min: 0, max: 250 },
        plot: {
            time: {
                color: { reverse: true, scale: "Hot" },
            },
            map: {
                layer: "PM2.5",
            },
        }
    },
};

const valueKeys = Object.keys(Props);