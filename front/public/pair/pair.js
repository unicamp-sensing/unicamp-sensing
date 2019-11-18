// ref.: http://bl.ocks.org/tnightingale/4718717

(function() {
    let extent, scale,
        classes = 9,
        scheme_id = "BuPu",
        reverse = false;
    scheme = colorbrewer[scheme_id][classes],
        container = L.DomUtil.get('map'),
        map = L.map(container).setView([-22.814, -47.069], 15);

    //////////////
    const tileUrlsOSMDE = "http://{s}.tile.openstreetmap.de/tiles/osmde/{z}/{x}/{y}.png";
    const tilesOSMDE = L.tileLayer(tileUrlsOSMDE, {
        maxZoom: 18,
        attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
    });

    const tileUrlSTL = "http://stamen-tiles-{s}.a.ssl.fastly.net/toner-lite/{z}/{x}/{y}.{ext}";
    const tilesSTL = L.tileLayer('http://stamen-tiles-{s}.a.ssl.fastly.net/toner-lite/{z}/{x}/{y}.{ext}', {
        attribution: 'Map tiles by <a href="http://stamen.com">Stamen Design</a>, <a href="http://creativecommons.org/licenses/by/3.0">CC BY 3.0</a> &mdash; Map data &copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>',
        subdomains: 'abcd',
        minZoom: 0,
        maxZoom: 20,
        ext: 'png'
    });

    const toggleMaps = {
        "Default": L.tileLayer('http://{s}.tile.stamen.com/toner/{z}/{x}/{y}.png', {
            attribution: '<a href="http://content.stamen.com/dotspotting_toner_cartography_available_for_download">Stamen Toner</a>, <a href="http://www.openstreetmap.org/">OpenStreetMap</a>, <a href="http://creativecommons.org/licenses/by-sa/2.0/">CC-BY-SA</a>',
            maxZoom: 17
        }),
        "OSM DE": tilesOSMDE,
        "Stamen Toner Lite": tilesSTL
    };
    toggleMaps['Default'].addTo(map);
    L.control.layers(toggleMaps).addTo(map);
    //////////////

    d3.json(container.dataset.source, function(collection) {
        L.pointsLayer(collection, {
            radius: get_radius,
            applyStyle: circle_style
        }).addTo(map);

        let chart = timeseries_chart(scheme)
            .x(get_time).xLabel("Time of data collection")
            .y(get_tmp).yLabel("Temperature (°C)")
            .brushmove(on_brush);

        // FIXME
        console.log(new Set(collection.features.map(function(feature) {
            return feature.properties.time.substring(0, 14)
        })));
        startTime = new Date(2019, 11 - 1, 12, 16); // FIXME we should use month - 1
        endTime = new Date(2019, 11 - 1, 14, 18); // FIXME we should use month - 1
        const shownFeatures = collection.features.filter(function(feature) {
            datetime = new Date(feature.properties.time);
            return startTime <= datetime && datetime <= endTime;
        });
        d3.select("body").datum(shownFeatures).call(chart);
        // d3.select("body").datum(collection.features).call(chart);
    });

    function get_time(d) {
        return d3.time.format.iso.parse(d.properties.time);
    }

    function get_tmp(d) {
        return d.properties.tmp;
    }

    function on_brush(brush) {
        let s = brush.extent();
        d3.selectAll(".circle").classed("selected", function(d) {
            let time = get_time(d);
            return s[0] <= time && time <= s[1];
        });
    }

    function get_radius(d) {
        const currentZoom = map.getZoom();
        return (
            (currentZoom <= 11) ? 6 :
            (currentZoom <= 13) ? 1 :
            (currentZoom <= 16) ? 3 :
            (currentZoom <= 17) ? 4 : 5);
    }

    function circle_style(circles) {
        if (!(extent && scale)) {
            extent = d3.extent(circles.data(), function(d) { return d.properties.tmp; }); // FIXME
            scale = d3.scale.log()
                .domain(reverse ? extent.reverse() : extent)
                .range(d3.range(classes));
            console.log(`scale: ${scale} classes: ${classes} range: ${d3.range(classes)}`);
        }
        circles.attr('opacity', 0.4)
            .attr('stroke', scheme[classes - 1])
            .attr('stroke-width', 1)
            .attr('fill', function(d) {
                return scheme[(scale(d.properties.tmp) * 10).toFixed()]; // FIXME
            });

        circles.on('click', function(d, i) {
            L.DomEvent.stopPropagation(d3.event);

            let t = '<h3><%- board %></h3>' +
                '<ul>' +
                '<li>Temperature: <%- tmp %>°C</li>' +
                '<li>Time:        <%- datetime %></li>' +
                '<li>Coordinates: (<%- lat %>, <%- lon %>)</li>' +
                '</ul>';

            let data = {
                board: d.board_MAC_addr,
                tmp: d.properties.tmp,
                datetime: d.properties.time,
                lat: d.properties.lat,
                lon: d.properties.lon
            };

            // NOTE: GeoJSON coordinates are [lon, lat]
            L.popup()
                .setLatLng([d.geometry.coordinates[1], d.geometry.coordinates[0]])
                .setContent(_.template(t, data))
                .openOn(map);
        });
    }

    function timeseries_chart(color) {
        const mapRect = d3.select('#map').node().getBoundingClientRect();

        let margin = { top: 5, right: 5, bottom: 40, left: 50 },
            width = mapRect.width - margin.left - margin.right,
            height = 1.5 * (mapRect.height / 7); // mapRect.height = 70% of screen

        let x = d3.time.scale().domain([new Date(2019, 10, 07, 17, 26), new Date(2019, 10, 07, 17, 38)]),
            y = d3.scale.linear(),
            x_label = "X",
            y_label = "Y",
            brush = d3.svg.brush().x(x).on("brush", _brushmove);

        let get_x = () => {},
            get_y = () => {}; // no op

        function timeseries(selection) {
            selection.each(function(d) {
                x.range([0, width - 20]);
                y.range([height, 0]);

                let series = d3.select(this).append("svg").attr("id", "unicamp-timeseries")
                    .attr("width", width + margin.left + margin.right)
                    .attr("height", height + margin.top + margin.bottom)
                    .append("g").attr("id", "date-brush")
                    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

                let x_axis = series.append("g")
                    .attr("class", "x axis")
                    .attr("transform", "translate(0," + height + ")");

                let y_axis = series.append("g")
                    .attr("class", "y axis");

                x_axis.append("text")
                    .attr("class", "label")
                    .attr("x", margin.left + width / 2)
                    .attr("y", 35)
                    .style("text-anchor", "end")
                    .text(x_label);

                y_axis.append("text")
                    .attr("class", "label")
                    .attr("transform", "rotate(-90)")
                    .attr("y", -45)
                    .attr("dy", ".71em")
                    .style("text-anchor", "end")
                    .text(y_label);

                series.append("clipPath")
                    .attr("id", "clip")
                    .append("rect")
                    .attr("width", width - 1)
                    .attr("height", height - .25)
                    .attr("transform", "translate(1,0)");

                series.append("g")
                    .attr("class", "brush")
                    .call(brush)
                    .selectAll("rect")
                    .attr("height", height)
                    .style("stroke-width", 1)
                    .style("stroke", color[color.length - 1])
                    .style("fill", color[2])
                    .attr("opacity", 0.4);

                // FIXME
                x_extent = d3.extent(d, get_x); // returns [min, max] values in the given iterable
                // console.log(`x_extent: ${x_extent}`);
                x_extent[0] = new Date(x_extent[0].setHours(x_extent[0].getHours() - 1));
                x_extent[1] = new Date(x_extent[1].setHours(x_extent[1].getHours() + 1));
                // console.log(`x_extent': ${x_extent}`);
                x.domain(x_extent);
                x_axis.call(d3.svg.axis().scale(x).orient("bottom"));

                // FIXME
                y_extent = d3.extent(d, get_y); // returns [min, max] values in the given iterable
                // console.log(`y_extent: ${y_extent}`);
                y_extent[0] -= 2;
                y_extent[1] += 2;
                // console.log(`y_extent: ${y_extent}`);
                y.domain(y_extent);
                y_axis.call(d3.svg.axis().scale(y).orient("left"));

                series.append("g").attr("class", "timeseries")
                    .attr("clip-path", "url(#clip)")
                    .selectAll("circle")
                    .data(d).enter()
                    .append("circle")
                    .style("stroke", color[color.length - 2])
                    .style("stroke-width", .5)
                    .style("fill", color[color.length - 1])
                    .attr("opacity", .4)
                    .attr("r", 2)
                    .attr("transform", function(d) {
                        return "translate(" + x(get_x(d)) + "," + y(get_y(d)) + ")";
                    });
            });
        }

        timeseries.x = function(accessor) {
            if (!arguments.length) {
                console.log(`ERROR at .x (arguments.length is ${arguments.length})`)
                return get_x;
            }
            get_x = accessor;
            return timeseries;
        };

        timeseries.y = function(accessor) {
            if (!arguments.length) {
                console.log(`ERROR at .y (arguments.length is ${arguments.length})`)
                return get_y;
            }
            get_y = accessor;
            return timeseries;
        };

        timeseries.xLabel = function(label) {
            if (!arguments.length) {
                console.log(`ERROR at .xLabel (arguments.length is ${arguments.length})`);
                return x_label;
            }
            x_label = label;
            return timeseries;
        }

        timeseries.yLabel = function(label) {
            if (!arguments.length) {
                console.log(`ERROR at .yLabel (arguments.length is ${arguments.length})`)
                return y_label;
            }
            y_label = label;
            return timeseries;
        }

        timeseries.brushmove = function(cb) {
            if (!arguments.length) {
                console.log(`ERROR at .brushmove (arguments.length is ${arguments.length})`)
                return brushmove;
            }
            brushmove = cb;
            return timeseries;
        };

        function _brushmove() {
            brushmove.call(null, brush);
        }

        return timeseries;
    }
}());