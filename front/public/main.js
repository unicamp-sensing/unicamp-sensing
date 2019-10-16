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

    plot_temperature();
    // plot_humidity();
    // plot_pm10();
    // plot_pm25();
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

const rand_color = () => Math.round(255 * Math.random());

function board_values_by_day_date(data) {
    const values_by_day_date = {};
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
                values_by_day_date[new Date(year, month, day)] = values;
            }
        }
    }
    return values_by_day_date;
}

async function plot_temperature() {
    const data = await getRawData();

    const values_by_day_date = board_values_by_day_date(data);
    datasets = [];
    for (const [day_date, values] of Object.entries(values_by_day_date)) {
        // get the value we're interested in
        tmp_values = values.map(function(val) { 
            return { x: val.x, y: val.y["tmp"] }
        }); // .filter(val => !!val.y)
        
        // generate a random color for the day
        const rgb = [rand_color(), rand_color(), rand_color()];
        datasets.push({
            label: formatDate(new Date(day_date)),
            data: tmp_values,
            backgroundColor: `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 0.2)`,
            borderColor: `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 1)`,
            borderWidth: 2,
            fill: false
        });
    }


    const ctx = document.getElementById("canvas-chart-tmp").getContext("2d");
    return new Chart(ctx, {
        type: "scatter",
        data: { datasets },
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
                    scaleLabel: { display: true, labelString: 'Temperature (°C)' }
                }]
            }
        }
    });
}

async function plot_humidity() {
    const data = await getRawData();

    const values_by_day_date = {};
    for (const year in data) {
        for (const month in data[year]) {
            for (const day in data[year][month]) {
                const values = [];
                for (const hour in data[year][month][day]) {
                    for (const min in data[year][month][day][hour]) {
                        for (const sec in data[year][month][day][hour][min]) {
                            for (const board in data[year][month][day][hour][min][sec]) {
                                const humidity = data[year][month][day][hour][min][sec][board].hum;
                                if (!!humidity) {
                                    const date = new Date(year, month, day, hour, min, sec);
                                    values.push({
                                        x: date,
                                        y: humidity
                                    });
                                }
                            }
                        }
                    }
                }
                values_by_day_date[new Date(year, month, day)] = values;
            }
        }
    }

    datasets = [];
    for (const [day_date, values] of Object.entries(values_by_day_date)) {
        // console.log(day_date);
        const rgb = [rand_color(), rand_color(), rand_color()];
        datasets.push({
            label: formatDate(new Date(day_date)),
            data: values,
            backgroundColor: `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 0.2)`,
            borderColor: `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 1)`,
            borderWidth: 2,
            fill: false
        });
    }

    const ctx = document.getElementById("canvas-chart-hum").getContext("2d");
    return new Chart(ctx, {
        type: "scatter",
        data: { datasets },
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
                    scaleLabel: { display: true, labelString: 'Humidity (%RH)' } // relative humidity
                }]
            }
        }
    });
}

async function plot_pm10() {
    const data = await getRawData();

    const values_by_day_date = {};
    for (const year in data) {
        for (const month in data[year]) {
            for (const day in data[year][month]) {
                const values = [];
                for (const hour in data[year][month][day]) {
                    for (const min in data[year][month][day][hour]) {
                        for (const sec in data[year][month][day][hour][min]) {
                            for (const board in data[year][month][day][hour][min][sec]) {
                                const pm10_conc = data[year][month][day][hour][min][sec][board].pm10;
                                if (!!pm10_conc) {
                                    const date = new Date(year, month, day, hour, min, sec);
                                    values.push({
                                        x: date,
                                        y: pm10_conc
                                    });
                                }
                            }
                        }
                    }
                }
                values_by_day_date[new Date(year, month, day)] = values;
            }
        }
    }

    datasets = [];
    for (const [day_date, values] of Object.entries(values_by_day_date)) {
        // console.log(day_date);
        const rgb = [rand_color(), rand_color(), rand_color()];
        datasets.push({
            label: formatDate(new Date(day_date)),
            data: values,
            backgroundColor: `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 0.2)`,
            borderColor: `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 1)`,
            borderWidth: 2,
            fill: false
        });
    }

    const ctx = document.getElementById("canvas-chart-pm10").getContext("2d");
    return new Chart(ctx, {
        type: "scatter",
        data: { datasets },
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
                    scaleLabel: { display: true, labelString: 'PM10 Concentration' }
                }]
            }
        }
    });
}

async function plot_pm25() {
    const data = await getRawData();

    const values_by_day_date = {};
    for (const year in data) {
        for (const month in data[year]) {
            for (const day in data[year][month]) {
                const values = [];
                for (const hour in data[year][month][day]) {
                    for (const min in data[year][month][day][hour]) {
                        for (const sec in data[year][month][day][hour][min]) {
                            for (const board in data[year][month][day][hour][min][sec]) {
                                const pm25_conc = data[year][month][day][hour][min][sec][board].pm25;
                                if (!!pm25_conc) {
                                    const date = new Date(year, month, day, hour, min, sec);
                                    values.push({
                                        x: date,
                                        y: pm25_conc
                                    });
                                }
                            }
                        }
                    }
                }
                values_by_day_date[new Date(year, month, day)] = values;
            }
        }
    }

    datasets = [];
    for (const [day_date, values] of Object.entries(values_by_day_date)) {
        // console.log(day_date);
        const rgb = [rand_color(), rand_color(), rand_color()];
        datasets.push({
            label: formatDate(new Date(day_date)),
            data: values,
            backgroundColor: `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 0.2)`,
            borderColor: `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 1)`,
            borderWidth: 2,
            fill: false
        });
    }

    const ctx = document.getElementById("canvas-chart-pm25").getContext("2d");
    return new Chart(ctx, {
        type: "scatter",
        data: { datasets },
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
                    scaleLabel: { display: true, labelString: 'PM2.5 Concentration' }
                }]
            }
        }
    });
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
