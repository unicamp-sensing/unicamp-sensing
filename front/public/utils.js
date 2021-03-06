
const $ = id => document.getElementById(id);

///////////////////////////////////////////////////////////////////////////////

const all = list => list.every(x => !!x); // true iff all values are truthy
const any = list => list.some(x => !!x);  // true iff at least one value is truthy
// obs.: false, 0, '', "", null, undefined and NaN are always falsy values

const randColor = () => Math.round(255 * Math.random());

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

///////////////////////////////////////////////////////////////////////////////

function exportCsvFile(header, rows, fileTitle) {
    const fileName = (fileTitle || "data") + ".csv";
    const csvFile = header + "\n" + rows.join("\n");
    const blob = new Blob([csvFile], {
        type: "text/plain;charset=utf-8"
    });
    saveAs(blob, fileName);
}

function rawDataToCsvArray(data) {
    let csvArray = ["year,month,day,hour,min,sec,board,humidity,temperature,pm10,pm2.5,lat,lon"];
    for (const year in data) {
        for (const month in data[year]) {
            for (const day in data[year][month]) {
                for (const hour in data[year][month][day]) {
                    for (const min in data[year][month][day][hour]) {
                        for (const sec in data[year][month][day][hour][min]) {
                            for (const board in data[year][month][day][hour][min][sec]) {
                                const { hum: humidity, tmp: temperature, ...boardValues } = data[year][month][day][hour][min][sec][board];
                                if (!humidity || !temperature) {
                                    console.log("invalid entry at:", year, month, day, hour, min, sec, board);
                                } else {
                                    const { pm10, pm25 } = boardValues;
                                    if (!pm10 || !pm25) {
                                        csvArray.push(`${year},${month},${day},${hour},${min},${sec},${board},${humidity},${temperature},,,,`);
                                    } else {
                                        const { lat, lon } = boardValues;
                                        if (!lat || !lon) {
                                            csvArray.push(`${year},${month},${day},${hour},${min},${sec},${board},${humidity},${temperature},${pm10},${pm25},,`);
                                        } else {
                                            csvArray.push(`${year},${month},${day},${hour},${min},${sec},${board},${humidity},${temperature},${pm10},${pm25},${lat},${lon}`);
                                        }
                                    }
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

function stopLoading(text) {
    text.innerHTML = "";
}