# Unicamp Sensing

**Unicamp Sensing** is a project developed by Computer Engineering students at [UNICAMP](https://www.unicamp.br/unicamp/)'s [Institute of Computing](https://ic.unicamp.br/en)
to evaluate the [walkability](https://en.wikipedia.org/wiki/Walkability) in the campus, through analysis of temperature, humidity and particulate matter levels.

Visualizations of the collected data can be seen at [unicamp-sensing.herokuapp.com](http://unicamp-sensing.herokuapp.com/).

## Background

This work is largely inspired by two projects from the [MIT Senseable City Lab](http://senseable.mit.edu/): 
- [City Scanner](http://senseable.mit.edu/cityscanner/)
- [Urban Sensing](http://senseable.mit.edu/urban-sensing/)

While the name comes directly from the latter, our project is more resemblant of the former: instead of using vehicles to capture the spatiotemporal variation in environmental indicators, which ends up collecting information on the roads, we "walk our sensors" to better understand the paths that are taken daily by students in the university.

This way, we aim to learn **where** and **how** to improve campus walkability, by analysing where are the places with the worst indicators.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Refer to [Docs](https://github.com/unicamp-sensing/unicamp-sensing/tree/master/docs#docs).

## Schematics

Data is gathered with: 
*  NodeMcu ESP8266 micro-controller;
*  DHT11 temperatyure & humidity sensor;
*  NEO-6MV2 GPS module;
*  DSM501A air particle sensor.


![Schematics](https://raw.githubusercontent.com/unicamp-sensing/unicamp-sensing/master/docs/schematic_projetos.png)

## License
[WTFPL](http://www.wtfpl.net/)
