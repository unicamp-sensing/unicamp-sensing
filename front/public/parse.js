
async function getRawData() {
    const dataRef = firebase.database().ref("/");
    const dataSnapshot = await dataRef.once("value");
    const data = dataSnapshot.val();
    return data;
}

function boardValuesByDayDate(data) {
    const valuesByDayDate = {};
    for (const year in data) {
        for (const month in data[year]) {
            for (const day in data[year][month]) {
                const values = [];
                for (const hour in data[year][month][day]) {
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
