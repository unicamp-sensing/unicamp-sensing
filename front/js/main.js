"use strict";

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
    console.log("Initializing Firebase...");
    firebase.initializeApp(firebaseConfig);

    $("button-download").addEventListener("click", download);

    plot();
}

async function plot() {
    const data = await getRawData();

    const values = [];
    for (const year in data) {
        for (const month in data[year]) {
            for (const day in data[year][month]) {
                for (const hour in data[year][month][day]) {
                    for (const min in data[year][month][day][hour]) {
                        for (const sec in data[year][month][day][hour][min]) {
                            for (const board in data[year][month][day][hour][min][sec]) {
                                const temperature = data[year][month][day][hour][min][sec][board].tmp;
                                if (!!temperature) {
                                    values.push({
                                        x: new Date(year, month, day, hour, min, sec),
                                        y: temperature
                                    });
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // FIXME
    const ctx = document.getElementById("canvas-chart").getContext("2d");
    const chart = new Chart(ctx, {
        type: "line",
        data: {
            datasets: [{
                label: "Average Temperature (°C)",
                data: values,
                backgroundColor: "rgba(255, 99, 132, 0.2)",
                borderColor: "rgba(255, 99, 132, 1)",
                borderWidth: 2,
                fill: false
            }]
        },
        options: {
            responsive: true,
            title: {
                display: true,
                text: 'Urban Sensing'
            },
            scales: {
                xAxes: [{
                    type: 'time',
                    display: true,
                    scaleLabel: {
                        display: true,
                        labelString: 'Date'
                    },
                    ticks: {
                        major: {
                            fontStyle: 'bold',
                            fontColor: '#FF0000'
                        }
                    }
                }],
                yAxes: [{
                    display: true,
                    scaleLabel: {
                        display: true,
                        labelString: 'Value'
                    }
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