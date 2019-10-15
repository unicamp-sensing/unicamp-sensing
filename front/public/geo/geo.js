const geoMap = L.map('geoMap').setView([-22.814, -47.069], 15);
drawMap();
// getData();

// async function getData() {
//     const response = await fetch("/api");
//     const data = await response.json();
//     for (const item of data) {
//         const marker = L.marker([item.lat, item.long]).addTo(geoMap);

//         let popupText = `The weather here at ${item.lat.toFixed(2)}&deg;,
//         ${item.long.toFixed(2)}&deg; is ${item.weather.summary} with
//         a temperature of ${item.weather.temperature}&deg; C.`;

//         try {
//             popupText += ` The concentration of particulate matter 
//             (${item.air_quality.parameter}) is ${item.air_quality.value} 
//             ${item.air_quality.unit} last read on ${item.air_quality.lastUpdated}`;
//         } catch (err) {
//             popupText += " (no air quality reading)";
//         }

//         marker.bindPopup(popupText);
//     }
//     console.log(data);
// }

function drawMap() {
    const tileUrl = 'https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png';
    const tiles = L.tileLayer(tileUrl, {
        attribution: "&copy; <a href=\"https://www.openstreetmap.org/copyright\">OpenStreetMap</a> contributors"
    });
    tiles.addTo(geoMap);
}