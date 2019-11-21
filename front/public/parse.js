async function getRawData() {
    const dataRef = firebase.database().ref("/");
    const dataSnapshot = await dataRef.once("value");
    const data = dataSnapshot.val();
    return data;
}



function boardValuesByDayDate(data, month_in_0_to_11=true) {
    const valuesByDayDate = {};
    for (const year in data) {
        if (parseInt(year) >= 2019) {
            for (const month in data[year]) {
                for (const day in data[year][month]) {
                    const values = [];
                    for (const hour in data[year][month][day]) {
                        for (const min in data[year][month][day][hour]) {
                            for (const sec in data[year][month][day][hour][min]) {
                                for (const board in data[year][month][day][hour][min][sec]) {
                                    //Date expects month in 0 to 11 range
                                    const date = new Date(year, month-(month_in_0_to_11? 1 : 0), day, hour, min, sec);
                                    if (sensorRangeValid(data[year][month][day][hour][min][sec][board])) {
                                        values.push({
                                            x: date,
                                            y: data[year][month][day][hour][min][sec][board]
                                        });
                                    }
                                }
                            }
                        }
                    }
                    valuesByDayDate[new Date(year, month-(month_in_0_to_11? 1 : 0), day)] = values;
                }
            }
        }
    }
    return valuesByDayDate;
}

function sensorRangeValid(values) {
    const inRange = (vKey) => Props[vKey].range.min <= values[vKey] && values[vKey] <= Props[vKey].range.max;
    return all(valueKeys.map(vKey => inRange(vKey)));
}
