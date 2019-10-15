const map = L.map("mapid").setView([-22.814, -47.069], 16);
initMap();

function initMap() {
    // get the stamen toner-lite tiles
    const tiles = L.tileLayer(
        "http://stamen-tiles-{s}.a.ssl.fastly.net/toner-lite/{z}/{x}/{y}.{ext}", {
            attribution: 'Map tiles by <a href="http://stamen.com">Stamen Design</a>, <a href="http://creativecommons.org/licenses/by/3.0">CC BY 3.0</a> — Map data © <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>',
            subdomains: "abcd",
            minZoom: 0,
            maxZoom: 20,
            ext: "png"
        });

    tiles.addTo(map);

    //disable scroll wheel zoom
    map.scrollWheelZoom.disable();
}
