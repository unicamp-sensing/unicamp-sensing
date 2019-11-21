initFirebase();

const geoMap = L.map("geoMap").setView(unicamp, 16);
drawMap(toggleMaps["Stamen Toner Lite"]);

const overlayMaps = createDataLayers();
var intervalID = setInterval(addDataStub, 5000);
L.control.layers(toggleMaps, overlayMaps).addTo(geoMap);

function addDataStub() {
    addData(overlayMaps);
    console.log('Updating overlayMaps')
}

///////////////////////////////////////////////////////////////////////////////

function drawMap(defaultTiles) {
    defaultTiles.addTo(geoMap);
}

function createDataLayers() {
    return {
        "Temperature": new L.layerGroup(),
        "Humidity":    new L.layerGroup(),
        "PM10":        new L.layerGroup(),
        "PM2.5":       new L.layerGroup()
    };
}

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
                                            L.circle([lat, lon], radius, 
                                                     { color: scaledColor, stroke: false, fillOpacity: 1.0 })
                                             .bindPopup(`${layer} = ${values[valueKey]}${Props[valueKey].unit} ` + 
                                                        `at (${lat}, ${lon})`)
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
