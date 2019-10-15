const map = L.map("mapid").setView([-22.814, -47.069], 16);
map.scrollWheelZoom.disable();

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

// layers
const red    = new L.layerGroup();
const blue   = new L.layerGroup();
const green  = new L.layerGroup();
const purple = new L.layerGroup();

addData();

const toggleMaps = {
    "OSM DE": tilesOSMDE,
    "Stamen Toner Lite": tilesSTL
};
const overlayMaps = {
    "red": red,
    "blue": blue,
    "green": green,
    "purple": purple
};
L.control.layers(toggleMaps, overlayMaps).addTo(map);


function addData() {
    // mock some data
    // FIXME TODO add real values from Firebase
    L.circle([37.795, -122.26], 500, {color:"red"}).addTo(red);
    L.circle([37.790, -122.26], 500, {color:"blue"}).addTo(blue);
    L.circle([37.785, -122.26], 500, {color:"green"}).addTo(green);
    L.circle([37.780, -122.26], 500, {color:"purple"}).addTo(purple);
}
