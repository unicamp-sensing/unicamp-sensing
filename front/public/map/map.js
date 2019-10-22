const tileUrlsOSMDE = "http://{s}.tile.openstreetmap.de/tiles/osmde/{z}/{x}/{y}.png";
const tilesOSMDE = L.tileLayer(tileUrlsOSMDE, {
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

///////////////////////////////////////////////////////////////////////////////

initFirebase();

const unicamp = [-22.814, -47.069];
const geoMap = L.map("geoMap").setView(unicamp, 16);
drawMap(toggleMaps["OSM DE"]);

const overlayMaps = createDataLayers();
// addMockData(overlayMaps);
addData(overlayMaps);
L.control.layers(toggleMaps, overlayMaps).addTo(geoMap);

///////////////////////////////////////////////////////////////////////////////

function drawMap(defaultTiles) {
    defaultTiles.addTo(geoMap);
    // geoMap.scrollWheelZoom.disable();
}

const valueKeyToColor = { "tmp": "red", "hum": "blue", "pm10": "green", "pm25": "purple" };
const valueKeyToLayer = { "tmp": "Temperature", "hum": "Humidity", "pm10": "PM10", "pm25": "PM2.5" };
const valueKeyToUnit = { "tmp": "°C", "hum": "%RH", "pm10": "", "pm25": "" };

function createDataLayers() {
    const red    = new L.layerGroup(); // tmp
    const blue   = new L.layerGroup(); // hum
    const green  = new L.layerGroup(); // pm10
    const purple = new L.layerGroup(); // pm2.5
    return {
        "Temperature": red,
        "Humidity": blue,
        "PM10": green,
        "PM2.5": purple
    };
}

async function addData(dataLayers) {
    const radius = 2;
    const data = await getRawData();
    for (const year in data) {
        for (const month in data[year]) {
            for (const day in data[year][month]) {
                for (const hour in data[year][month][day]) {
                    for (const min in data[year][month][day][hour]) {
                        for (const sec in data[year][month][day][hour][min]) {
                            for (const board in data[year][month][day][hour][min][sec]) {
                                const boardData = data[year][month][day][hour][min][sec][board];
                                // check if boardData is actually a dictionary
                                if (boardData.constructor == Object) {
                                    const { lat, lon, ...values } = boardData;
                                    // check if it contains "lat" and "lon" keys
                                    if (!!lat && !!lon) {
                                        for (const valueKey of Object.keys(values)) {
                                            const color = valueKeyToColor[valueKey]
                                            if (!!color) {
                                                L.circle(
                                                  [lat, lon],
                                                  radius,
                                                  { color: color, stroke: false })
                                                  .bindPopup(`${valueKeyToLayer[valueKey]} = ${values[valueKey]}${valueKeyToUnit[valueKey]} at [${lat}, ${lon}]`)
                                                  .addTo(dataLayers[valueKeyToLayer[valueKey]]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

function addMockData(dataLayers) {
    const radius = 10;
    const addLatLon = (latLon1, latLon2) => [latLon1[0] + latLon2[0], latLon1[1] + latLon2[1]];
    // mock some data for now
    L.circle(addLatLon(unicamp, [0.001, 0]), radius, { color: "red" }).addTo(dataLayers["red"]);
    L.circle(addLatLon(unicamp, [0.002, 0]), radius, { color: "blue" }).addTo(dataLayers["blue"]);
    L.circle(addLatLon(unicamp, [0.003, 0]), radius, { color: "green" }).addTo(dataLayers["green"]);
    L.circle(addLatLon(unicamp, [0.004, 0]), radius, { color: "purple" }).addTo(dataLayers["purple"]);
}
