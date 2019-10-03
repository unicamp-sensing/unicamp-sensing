const express = require("express");
const fetch = require("node-fetch");
require("dotenv").config();

const app = express();
const port = process.env.PORT || 3000;
app.listen(port, () => console.log(`Listening at port ${port}...`));
app.use(express.static("public"));
// app.use(express.json({ limit: "1mb" }));
