// Original code from:
// https://gist.github.com/dannypule/48418b4cd8223104c6c92e3016fc0f61#file-json_to_csv-js

function convertToCSV(objArray) {
    console.log(typeof objArray);
    var array = typeof objArray != 'object' ? JSON.parse(objArray) : objArray;
    var str = '';

    for (key in array) {
        console.log(key);
        console.log(array[key]);
        break;
    }

    for (var i = 0; i < array.length; i++) {
        var line = '';
        for (var index in array[i]) {
            if (line != '') line += ',';
            line += array[i][index];
        }
        str += line + '\r\n';
        break;
    }
    return str;
}

function exportCSVFile(headers, items, fileTitle) {
    // if (headers) {
    //     items.unshift(headers);
    // }

    // Convert Object to JSON
    var jsonObject = JSON.stringify(items);

    var csv = this.convertToCSV(jsonObject);

    console.log(csv);

    var exportedFilename = fileTitle + '.csv' || 'export.csv';


    // var blob = new Blob([csv], { type: 'text/csv;charset=utf-8;' });
    // if (navigator.msSaveBlob) { // IE 10+
    //     navigator.msSaveBlob(blob, exportedFilename);
    // } else {
    //     var link = document.createElement("a");
    //     if (link.download !== undefined) { // feature detection
    //         // Browsers that support HTML5 download attribute
    //         var url = URL.createObjectURL(blob);
    //         link.setAttribute("href", url);
    //         link.setAttribute("download", exportedFilename);
    //         link.style.visibility = 'hidden';
    //         document.body.appendChild(link);
    //         link.click();
    //         document.body.removeChild(link);
    //     }
    // }
}

function exportJSONFile(object, fileTitle) {
    var jsonObject = JSON.stringify(object);

    var exportedFilename = fileTitle + '.json' || 'export.json';

    var blob = new Blob([jsonObject], { type: 'text/json;charset=utf-8;' });
    if (navigator.msSaveBlob) { // IE 10+
        navigator.msSaveBlob(blob, exportedFilename);
    } else {
        var link = document.createElement("a");
        if (link.download !== undefined) { // feature detection
            // Browsers that support HTML5 download attribute
            var url = URL.createObjectURL(blob);
            link.setAttribute("href", url);
            link.setAttribute("download", exportedFilename);
            link.style.visibility = 'hidden';
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
        }
    }
}

function download() {
    // TODO Show user that we are doing something, popup or some banner
    // alert("Preparing download");
    console.log("Preparing download");

    // Get the data
    const all_data_ref = firebase.database().ref("/");
    all_data_ref.once("value")
           .then(function (snapshot) {
            let values = snapshot.val(); // "last"
            const keys = Object.keys(values); // FIXME temporary workaround for current firebase data format

            // Download
            // exportJSONFile(values, "export");
            exportCSVFile(keys, values, "export");
    })

    
}

window.addEventListener('load', init, false);

function init() {
    document.getElementById("button-download").addEventListener("click", download)
}