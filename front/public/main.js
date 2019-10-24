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

    for (const [dayDate, boardValues] of Object.entries(valuesByDayDate)) {
        for (const valueKey of Object.keys(datasets)) {
            // get the value we're interested in
            values = boardValues.map(function(val) {
                return { x: val.x, y: val.y[valueKey] }
            }).filter(val => !!val.y);

            // generate a random color for the day
            const rgb = [randColor(), randColor(), randColor()];
            
            datasets[valueKey].push({
                label: formatDate(new Date(dayDate)),
                data: values,
                backgroundColor: `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 0.2)`,
                borderColor: `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 1)`,
                borderWidth: 2,
                fill: false
            });
        }
    }

    charts = {};
    for (const valueKey of Object.keys(datasets)) {
        const ctx = $(`canvas-chart-${valueKey}`).getContext("2d");
        const valueDatasets = datasets[valueKey];

        charts[valueKey] = new Chart(ctx, {
            type: "scatter",
            data: { datasets: valueDatasets },
            options: {
                responsive: true,
                title: { display: true, text: 'Urban Sensing' }, // FIXME pass a list of titles
                scales: {
                    xAxes: [{
                        type: 'time',
                        display: true,
                        scaleLabel: { display: true, labelString: 'Date' },
                        ticks: { major: { fontStyle: 'bold', fontColor: '#FF0000' } }
                    }],
                    yAxes: [{
                        display: true,
                        scaleLabel: { display: true, labelString: labels[valueKey] }
                    }]
                }
            }
        });
    }
    return charts;
}

async function download() {
    console.log("Downloading data from Firebase...");
    const data = await getRawData();

    const [header, ...rows] = rawDataToCsvArray(data);
    exportCsvFile(header, rows);
}