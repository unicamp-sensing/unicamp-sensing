# Docs
## Firebase data format
```bash
+-- [year]
|   +-- [month]
|   |   +-- [day]
|   |   |   +-- [hour]
|   |   |   |   +-- [minute]
|   |   |   |   |   +-- [second]
|   |   |   |   |   |   +-- [board_MAC_addr]
|   |   |   |   |   |   |   +-- hum: [humidity]
|   |   |   |   |   |   |   +-- tmp: [temperature]
```
## Running the website locally
Once you have [Node.js](https://nodejs.org/en/) installed and the repo cloned:
- `$ cd path/to/urban-sensing/front/`
- `$ npm install`
- `$ node index.js`
- Open `http://localhost:PORT/`, where `PORT` will be printed at the terminal after running the command above
