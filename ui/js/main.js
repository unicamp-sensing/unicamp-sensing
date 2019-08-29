// Your web app's Firebase configuration
var firebaseConfig = {
    apiKey: "AIzaSyATSHBpjnyHaTb8bF-NmWrzam4KZ5Amrh0",
    authDomain: "teste-bb0d8.firebaseapp.com",
    databaseURL: "https://teste-bb0d8.firebaseio.com",
    projectId: "teste-bb0d8",
    storageBucket: "teste-bb0d8.appspot.com",
    messagingSenderId: "701434842650",
    appId: "1:701434842650:web:fdf7e8b3d377c1b9"
};


window.addEventListener('load', init, false);

function init() {
    // Initialize Firebase
    firebase.initializeApp(firebaseConfig);

    let humidity_text = document.getElementById("humidade")
    console.log(humidity_text)

    var ref = firebase.database().ref("hum");

    setInterval(() => {
        ref.once("value")
        .then(function(snapshot) {
            let values = snapshot.val(); // "last"
            console.log(values)
            humidity_text.innerHTML = values[7]
        });
    }, 1000);
}