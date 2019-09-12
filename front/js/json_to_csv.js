'use strict';

// call init() once the whole page has loaded
window.addEventListener("load", init, false);

///////////////////////////////////////////////////////////////////////////////

function init() {
    document.getElementById("button-download").addEventListener("click", download);
}

async function download() {
    console.log("Downloading data from Firebase...");

    const dataRef = firebase.database().ref("/");
    const dataSnapshot = await dataRef.once("value");
    const data = dataSnapshot.val();

    console.log(data);

    const [header, ...rows] = dataToCsvArray(data);
    exportCsvFile(header, rows);
}

function dataToCsvArray(data) {
    let csvArray = ["year,month,day,hour,board,humidity,temperature"];
    for (const year in data) {
        for (const month in data[year]) {
            for (const day in data[year][month]) {
                for (const hour in data[year][month][day]) {
                    for (const min_sec in data[year][month][day][hour]) {
                        // const [min, sec] = min_sec.split(':');
                        for (const board in data[year][month][day][hour][min_sec]) {
                            const { hum: humidity, tmp: temperature } = data[year][month][day][hour][min_sec][board];
                            if (!humidity || !temperature) {
                                console.log("invalid entry at:", year, month, day, hour, min_sec);
                            } else {
                                csvArray.push(`${year},${month},${day},${hour},${board},${humidity},${temperature}`);
                            }
                        }
                    }
                }
            }
        }
    }
    return csvArray;
}

function exportCsvFile(header, rows, fileTitle) {
    const fileName = (fileTitle || "data") + ".csv";

    const csvFile = header + "\n" + rows.join("\n");
    
    const blob = new Blob([csvFile], {
        type: "text/plain;charset=utf-8"
    });
    saveAs(blob, fileName);
}

// ref.: https://firebase.google.com/docs/reference/js/firebase.database.Reference