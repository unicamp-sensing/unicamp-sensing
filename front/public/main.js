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

const valueKeyToRange = {
    "tmp":  {"min": 0, "max": 40}, // temperature
    "hum":  {"min": 0, "max": 100}, // humidity
    "pm10": {"min": 0, "max": 80}, // PM 10
    "pm25": {"min": 0, "max": 80}  // PM 2.5
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
            // len = xs.length
            // console.log(xs.length);
            console.log(ys);
            // generate a random color for the day
            const rgb = [randColor(), randColor(), randColor()];

            datasets[valueKey].push({
                mode: "markers",
                type: "scatter",
                name: moment(dayDate.toString()).format('DD/MM/YY'),
                x: xs,
                y: ys,

                marker: {
                    color: ys,
                    cmax: valueKeyToRange[valueKey].max,
                    cmin: valueKeyToRange[valueKey].min
                }
            });
        }
    }

    var layout = {
        xaxis: {
            range: ['2019-10-10', '2019-11-10'], // FIXME
            type: 'date'
        }
    };

    Plotly.newPlot("div-tmp", datasets['tmp'], { title: "Temperature (°C)", ...layout });
    Plotly.newPlot("div-hum", datasets['hum'], { title: "Humidity (%RH)", ...layout });
}

async function download() {
    console.log("Downloading data from Firebase...");
    const data = await getRawData();

    const [header, ...rows] = rawDataToCsvArray(data);
    exportCsvFile(header, rows);
}
