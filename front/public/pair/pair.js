// ref.: http://bl.ocks.org/tnightingale/4718717

(function() {
    let extent, scale,
        classes = 9,
        scheme_id = "BuPu",
        reverse = false,
        scheme = colorbrewer[scheme_id][classes],
        container = L.DomUtil.get('map'),
        map = L.map(container).setView(unicamp, 16);

    toggleMaps["Stamen Toner Lite"].addTo(map);
    L.control.layers(toggleMaps).addTo(map);

    d3.json(container.dataset.source, function(collection) {
        L.pointsLayer(collection, {
            radius: getRadius,
            applyStyle: circleStyle
        }).addTo(map);

        let chart = timeseriesChart(scheme)
            .x(getTime).xLabel("Time of data collection")
            .y(get_tmp).yLabel("Temperature (°C)")
            .brushmove(onBrush);

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
        // d3.select("body").datum(shownFeatures).call(chart);
        d3.select("body").datum(collection.features).call(chart);
    });

    function getTime(d) {
        return d3.time.format.iso.parse(d.properties.time);
    }

    function get_tmp(d) {
        return d.properties.tmp;
    }

    function onBrush(brush) {
        let s = brush.extent(); // [min, max]
        d3.selectAll(".circle").classed("selected", function(d) {
            let time = getTime(d);
            return s[0] <= time && time <= s[1]; // min <= time <= max
        });
    }

    function getRadius(d) {
        const currentZoom = map.getZoom();
        return (
            (currentZoom <= 11) ? 6 :
            (currentZoom <= 13) ? 1 :
            (currentZoom <= 16) ? 3 :
            (currentZoom <= 17) ? 4 : 5);
    }

    function circleStyle(circles) {
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
                datetime: moment(new Date(d.properties.time)).format('MMMM Do YYYY, h:mm:ss a'),
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

    function timeseriesChart(color) {
        const mapRect = d3.select('#map').node().getBoundingClientRect();

        let margin = { top: 5, right: 5, bottom: 40, left: 50 },
            width = mapRect.width - margin.left - margin.right,
            height = 1.5 * (mapRect.height / 7); // mapRect.height = 70% of screen

        let x = d3.time.scale(),
            y = d3.scale.linear(),
            x_label = "X",
            y_label = "Y",
            brush = d3.svg.brush().x(x).on("brush", _brushmove);

        let getX = () => {}, getY = () => {}; // no op

        function timeseries(selection) {
            selection.each(function(d) {
                x.range([5, width - 10]);
                y.range([height, 0]);

                let series = d3.select(this).append("svg").attr("id", "unicamp-timeseries")
                    .attr("width", width + margin.left + margin.right)
                    .attr("height", height + margin.top + margin.bottom)
                    .append("g").attr("id", "date-brush")
                    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

                let xAxis = series.append("g")
                    .attr("class", "x axis")
                    .attr("transform", "translate(0," + height + ")");

                let yAxis = series.append("g")
                    .attr("class", "y axis");

                xAxis.append("text")
                    .attr("class", "label")
                    .attr("x", margin.left + width / 2)
                    .attr("y", 35)
                    .style("text-anchor", "end")
                    .text(x_label);

                yAxis.append("text")
                    .attr("class", "label")
                    .attr("transform", "rotate(-90)")
                    .attr("x", -30)
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
                x_extent = d3.extent(d, getX); // returns [min, max] values in the given iterable
                // console.log(`x_extent: ${x_extent}`);
                x_extent[0] = new Date(x_extent[0].setHours(x_extent[0].getHours() - 1));
                x_extent[1] = new Date(x_extent[1].setHours(x_extent[1].getHours() + 1));
                // console.log(`x_extent': ${x_extent}`);
                x.domain(x_extent);
                xAxis.call(d3.svg.axis().scale(x).orient("bottom"));

                // FIXME
                y_extent = d3.extent(d, getY); // returns [min, max] values in the given iterable
                // console.log(`y_extent: ${y_extent}`);
                y_extent[0] -= 2;
                y_extent[1] += 2;
                // console.log(`y_extent: ${y_extent}`);
                y.domain(y_extent);
                yAxis.call(d3.svg.axis().scale(y).orient("left"));

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
                        return "translate(" + x(getX(d)) + "," + y(getY(d)) + ")";
                    });
            });
        }

        timeseries.x = function(accessor) {
            if (!arguments.length) return getX;
            getX = accessor;
            return timeseries;
        };

        timeseries.y = function(accessor) {
            if (!arguments.length) return getY;
            getY = accessor;
            return timeseries;
        };

        timeseries.xLabel = function(label) {
            if (!arguments.length) return x_label;
            x_label = label;
            return timeseries;
        }

        timeseries.yLabel = function(label) {
            if (!arguments.length) return y_label;
            y_label = label;
            return timeseries;
        }

        timeseries.brushmove = function(cb) {
            if (!arguments.length) return brushmove;
            brushmove = cb;
            return timeseries;
        };

        function _brushmove() {
            brushmove.call(null, brush);
        }

        return timeseries;
    }
}());