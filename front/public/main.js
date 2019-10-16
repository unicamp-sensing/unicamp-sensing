// call init() once the whole page has loaded
window.addEventListener("load", init, false);

///////////////////////////////////////////////////////////////////////////////

const $ = id => document.getElementById(id);

const firebaseConfig = {
    apiKey: "AIzaSyATSHBpjnyHaTb8bF-NmWrzam4KZ5Amrh0",
    authDomain: "teste-bb0d8.firebaseapp.com",
    databaseURL: "https://teste-bb0d8.firebaseio.com",
    projectId: "teste-bb0d8",
    storageBucket: "teste-bb0d8.appspot.com",
    messagingSenderId: "701434842650",
    appId: "1:701434842650:web:fdf7e8b3d377c1b9"
};

function init() {
    firebase.initializeApp(firebaseConfig);

    $("button-download").addEventListener("click", download);

    charts = plotData({
        "tmp":  "Temperature (°C)",
        "hum":  "Humidity (%RH)", // relative humidity
        "pm10": "PM10 Concentration",
        "pm25": "PM2.5 Concentration"
    });
}

function formatDate(date) {
    const monthNames = [
        "January", "February", "March",
        "April", "May", "June", "July",
        "August", "September", "October",
        "November", "December"
    ];

    const day = date.getDate();
    const monthIndex = date.getMonth();
    const year = date.getFullYear();

    return day + ' ' + monthNames[monthIndex] + ' ' + year;
}

const randColor = () => Math.round(255 * Math.random());

function boardValuesByDayDate(data) {
    const valuesByDayDate = {};
    for (const year in data) {
        for (const month in data[year]) {
            for (const day in data[year][month]) {
                const values = [];
                for (const hour in data[year][month][day]) {
                    if (day == 17 && hour == 16) continue;
                    for (const min in data[year][month][day][hour]) {
                        for (const sec in data[year][month][day][hour][min]) {
                            for (const board in data[year][month][day][hour][min][sec]) {
                                const date = new Date(year, month, day, hour, min, sec);
                                values.push({
                                    x: date,
                                    y: data[year][month][day][hour][min][sec][board]
                                });
                            }
                        }
                    }
                }
                valuesByDayDate[new Date(year, month, day)] = values;
            }
        }
    }
    return valuesByDayDate;
}

async function plotData(labels) {
    const data = await getRawData();

    const valuesByDayDate = boardValuesByDayDate(data);
    datasets = {
        "tmp":  [], // temperature
        "hum":  [], // humidity
        "pm10": [], // PM 10
        "pm25": []  // PM 2.5
    };

    for (const [dayDate, boardValues] of Object.entries(valuesByDayDate)) {
        for (const valueKey of Object.keys(datasets)) {
            // get the value we're interested in
            values = boardValues.map(function(val) {
                return { x: val.x, y: val.y[valueKey] }
            }); // .filter(val => !!val.y)

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
        console.log(valueKey);
        const ctx = $(`canvas-chart-${valueKey}`).getContext("2d");
        const valueDatasets = datasets[valueKey];

        charts[valueKey] = new Chart(ctx, {
            type: "scatter",
            data: { datasets: valueDatasets },
            options: {
                responsive: true,
                title: { display: true, text: 'Urban Sensing' },
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

async function getRawData() {
    const dataRef = firebase.database().ref("/");
    const dataSnapshot = await dataRef.once("value");
    const data = dataSnapshot.val();
    return data;
}

async function download() {
    console.log("Downloading data from Firebase...");
    const data = await getRawData();
    console.log(data);

    const [header, ...rows] = rawDataToCsvArray(data);
    exportCsvFile(header, rows);
}

function exportCsvFile(header, rows, fileTitle) {
    const fileName = (fileTitle || "data") + ".csv";
    const csvFile = header + "\n" + rows.join("\n");
    const blob = new Blob([csvFile], {
        type: "text/plain;charset=utf-8"
    });
    saveAs(blob, fileName);
}

function rawDataToCsvArray(data) {
    let csvArray = ["year,month,day,hour,min,sec,board,humidity,temperature"];
    for (const year in data) {
        for (const month in data[year]) {
            for (const day in data[year][month]) {
                for (const hour in data[year][month][day]) {
                    for (const min in data[year][month][day][hour]) {
                        for (const sec in data[year][month][day][hour][min]) {
                            for (const board in data[year][month][day][hour][min][sec]) {
                                const { hum: humidity, tmp: temperature } = data[year][month][day][hour][min][sec][board];
                                if (!humidity || !temperature) {
                                    console.log("invalid entry at:", year, month, day, hour, min, sec, board);
                                } else {
                                    csvArray.push(`${year},${month},${day},${hour},${min},${sec},${board},${humidity},${temperature}`);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return csvArray;
}