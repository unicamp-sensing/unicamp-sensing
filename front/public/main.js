// call init() once the whole page has loaded
window.addEventListener("load", init, false);

///////////////////////////////////////////////////////////////////////////////

function init() {
    initFirebase();

    $("button-download").addEventListener("click", download);

    charts = plotData({
        "tmp": "Temperature (°C)",
        "hum": "Humidity (%RH)", // relative humidity
        "pm10": "PM10 Concentration",
        "pm25": "PM2.5 Concentration"
    });
}


async function plotData(labels) {
    const data = await getRawData();

    const valuesByDayDate = boardValuesByDayDate(data);
    datasets = {
        "tmp": [], // temperature
        "hum": [], // humidity
        "pm10": [], // PM 10
        "pm25": [] // PM 2.5
    };

    // const values = Object.values(valuesByDayDate).flat();
    // const dates = values.map((val) => val.x);

    for (const [dayDate, boardValues] of Object.entries(valuesByDayDate)) {
        for (const valueKey of Object.keys(datasets)) {
            // get the value we're interested in
            values = boardValues.map(function(val) {
                return { x: val.x, y: val.y[valueKey] }
            }).filter(val => !!val.y);

            xs = values.map((val) => val.x);
            ys = values.map((val) => val.y);
            // generate a random color for the day
            const rgb = [randColor(), randColor(), randColor()];

            datasets[valueKey].push({
                mode: "markers",
                type: "scatter",
                name: moment(dayDate.toString()).format('DD/MM/YY'),
                x: xs,
                y: ys,
                showlegend: false,

                marker: {
                    color: ys,
                    showscale: true,
                    colorscale: Props[valueKey].plot.time.color.scale,
                    reversescale: Props[valueKey].plot.time.color.reverse,
                    cmax: Props[valueKey].range.max,
                    cmin: Props[valueKey].range.min
                }
            });
        }
    }

    var layout = {
        xaxis: {
            type: 'date'
        }
    };

    Plotly.newPlot("div-tmp", datasets['tmp'], { title: "Temperature (°C)", ...layout });
    Plotly.newPlot("div-hum", datasets['hum'], { title: "Humidity (%RH)", ...layout });
    Plotly.newPlot("div-pm10", datasets['pm10'], { title: "Particle Matter 10", ...layout });
    Plotly.newPlot("div-pm25", datasets['pm25'], { title: "Particle Matter 2.5", ...layout });
    stopLoading(document.getElementById("loading"));
}

async function download() {
    console.log("Downloading data from Firebase...");
    const data = await getRawData();

    const [header, ...rows] = rawDataToCsvArray(data);
    exportCsvFile(header, rows);
}
