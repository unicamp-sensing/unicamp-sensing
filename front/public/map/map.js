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

const valueKeyToChromaScale = {
    tmp:  chroma.scale(['yellow', 'red']),
    hum:  chroma.scale(['AliceBlue', 'DarkBlue']),
    pm10: chroma.scale(['Chartreuse', 'SaddleBrown']),
    pm25: chroma.scale(['Chartreuse', 'SaddleBrown'])
};

const valueKeyToD3Scale = {
    tmp:  d3.scaleLinear().domain([Props['tmp'].range.min, Props['tmp'].range.max]).range([0, 1]),
    hum:  d3.scaleLinear().domain([Props['hum'].range.min, Props['hum'].range.max]).range([0, 1]),
    pm10: d3.scaleLinear().domain([Props['pm10'].range.min, Props['pm10'].range.max]).range([0, 1]),
    pm25: d3.scaleLinear().domain([Props['pm25'].range.min, Props['pm25'].range.max]).range([0, 1])
};

initFirebase();

const unicamp = [-22.814, -47.069];
const geoMap = L.map("geoMap").setView(unicamp, 16);
drawMap(toggleMaps["Stamen Toner Lite"]);

const overlayMaps = createDataLayers();
addData(overlayMaps);
L.control.layers(toggleMaps, overlayMaps).addTo(geoMap);

///////////////////////////////////////////////////////////////////////////////

function drawMap(defaultTiles) {
    defaultTiles.addTo(geoMap);
}

function createDataLayers() {
    return {
        "Temperature": new L.layerGroup(),
        "Humidity": new L.layerGroup(),
        "PM10": new L.layerGroup(),
        "PM2.5": new L.layerGroup()
    };
    // const layers = {};
    // for (valueKey of valueKeys) {
    //     layers[Props[valueKey].plot.map.layer] = new L.layerGroup();
    // }
    // return layers;
}

function getScaledColor(valueKey, value) {
    colorScale = valueKeyToChromaScale[valueKey];
    adjustScale = valueKeyToD3Scale[valueKey];
    return colorScale(adjustScale(value));
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
                                        for (const valueKey of Object.keys(values).filter(it => valueKeys.includes(it))) {
                                            const layer = Props[valueKey].plot.map.layer;
                                            const scaledColor = getScaledColor(valueKey, boardData[valueKey]);
                                            L.circle([lat, lon], radius, { color: scaledColor, stroke: false, fillOpacity: 1.0 })
                                             .bindPopup(`${layer} = ${values[valueKey]}${Props[valueKey].unit} at [${lat}, ${lon}]`)
                                             .addTo(dataLayers[layer]);
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