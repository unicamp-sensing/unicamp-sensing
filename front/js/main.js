'use strict';

// Your web app's Firebase configuration
const firebaseConfig = {
    apiKey: "AIzaSyATSHBpjnyHaTb8bF-NmWrzam4KZ5Amrh0",
    authDomain: "teste-bb0d8.firebaseapp.com",
    databaseURL: "https://teste-bb0d8.firebaseio.com",
    projectId: "teste-bb0d8",
    storageBucket: "teste-bb0d8.appspot.com",
    messagingSenderId: "701434842650",
    appId: "1:701434842650:web:fdf7e8b3d377c1b9"
};

window.addEventListener('load', init, false);

function update_text_with_ref(text, ref, index, strs) {
    // FIXME improve 'index' and 'strs' solution
    ref
        .once("value")
        .then(function (snapshot) {
            let values = snapshot.val(); // "last"
            const keys = Object.keys(values); // FIXME temporary workaround for current firebase data format
            console.log(`${strs[0]} values: ${values}`);
            const key = keys[0];

            const value_keys = Object.keys(values[key]);
            const i = index % value_keys.length
            const value = values[keys[0]][value_keys[i]];
            text.innerHTML = `${value} ${strs[1]} (${key}:${value_keys[i]})`;

        });
}

function init() {
    // Initialize Firebase
    firebase.initializeApp(firebaseConfig);

    const humidity_text = document.getElementById("humidade");
    const temperature_text = document.getElementById("temperatura");

    const humidity_ref = firebase.database().ref("hum");
    const temperature_ref = firebase.database().ref("tmp");

    let index = 0;
    setInterval(() => {
        update_text_with_ref(humidity_text, humidity_ref, index, ["hum", "RH"]);
        update_text_with_ref(temperature_text, temperature_ref, index, ["tmp", "°C"]);
        index++;
    }, 1000);
}