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

///////////////////////////////////////////////////////////////////////////////

const tileUrlsOSMDE = "http://{s}.tile.openstreetmap.de/tiles/osmde/{z}/{x}/{y}.png";
const tilesOSMDE = L.tileLayer(tileUrlsOSMDE, {
    minZoom: 0,
    maxZoom: 18,
    attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
});

const tileUrlSTL = "http://stamen-tiles-{s}.a.ssl.fastly.net/toner-lite/{z}/{x}/{y}.{ext}";
const tilesSTL = L.tileLayer('http://stamen-tiles-{s}.a.ssl.fastly.net/toner-lite/{z}/{x}/{y}.{ext}', {
    attribution: 'Map tiles by <a href="http://stamen.com">Stamen Design</a>, <a href="http://creativecommons.org/licenses/by/3.0">CC BY 3.0</a> &mdash; Map data &copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>',
    subdomains: 'abcd',
    minZoom: 0,
    maxZoom: 20,
    ext: 'png'
});

const toggleMaps = {
    "OSM DE": tilesOSMDE,
    "Stamen Toner Lite": tilesSTL
};

const unicamp = [-22.814, -47.069];
