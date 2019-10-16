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

const unicamp = [-22.814, -47.069];
const geoMap = L.map("geoMap").setView(unicamp, 16);
drawMap(toggleMaps["OSM DE"]);

const overlayMaps = createDataLayers();
addData(overlayMaps);
L.control.layers(toggleMaps, overlayMaps).addTo(geoMap);

///////////////////////////////////////////////////////////////////////////////

function drawMap(defaultTiles) {
    defaultTiles.addTo(geoMap);
    geoMap.scrollWheelZoom.disable();
}

function createDataLayers() {
    const red    = new L.layerGroup();
    const blue   = new L.layerGroup();
    const green  = new L.layerGroup();
    const purple = new L.layerGroup();
    return {
        "red": red,
        "blue": blue,
        "green": green,
        "purple": purple
    };
}

// FIXME TODO add real values from Firebase
function addData(dataLayers) {
    // mock some data for now
    radius = 30;
    const addLatLng = (latLng1, latLng2) => [latLng1[0] + latLng2[0], latLng1[1] + latLng2[1]];
    L.circle(addLatLng(unicamp, [0.001, 0]), radius, { color: "red" }).addTo(dataLayers["red"]);
    L.circle(addLatLng(unicamp, [0.002, 0]), radius, { color: "blue" }).addTo(dataLayers["blue"]);
    L.circle(addLatLng(unicamp, [0.003, 0]), radius, { color: "green" }).addTo(dataLayers["green"]);
    L.circle(addLatLng(unicamp, [0.004, 0]), radius, { color: "purple" }).addTo(dataLayers["purple"]);
}
