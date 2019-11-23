// call init() once the whole page has loaded
window.addEventListener("load", init, false);

///////////////////////////////////////////////////////////////////////////////

function init() {
    initFirebase();

    $("button-download").addEventListener("click", download);
}


async function download() {
    console.log("Downloading data from Firebase...");
    const data = await getRawData();

    const [header, ...rows] = rawDataToCsvArray(data);
    exportCsvFile(header, rows);
}
