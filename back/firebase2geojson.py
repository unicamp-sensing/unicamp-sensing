# Firebase structure:
# {
#   <year> : {
#     <month> : {
#       <day> : {
#         <hour> : {
#           <minute> : {
#             <second> : {
#               <board_MAC_addr> : {
#                 "hum" : <humidity>, "tmp" : <temperature>,
#                 "lat" : <latitude>, "lon" : <longitude>,
#                 "pm10" : <PM10>,    "pm25" : <PM2.5>
#               }, ...
#           ⋮
#   }, ...
# }

# GeoJSON structure:
# {
#    "type": "FeatureCollection",
#    "features": [
#       {
#          "type": "Feature",
#          "geometry": { "type": "Point", "coordinates": [<lat>, <lon>] },
#          "properties": {
#             "time": "<year>-<month>-<day>T<hour>:<minute>:<second>Z",
#             "hum": <humidity>, "tmp": <temperature>,
#             "lat": <latitude>, "lon": <longitude>,
#             "pm10": <PM10>,    "pm25": <PM2.5>
#          }
#       }, ...
#    ]
# }
# obs.: time: "yyyy-MM-dd'T'HH:mm:ss.SSS'Z'" (Z means "zero hour offset", i.e. UTC)

import sys

def convert(json):
    ''' Converts `json` from a Firebase structured string to the GeoJSON format '''
    geo_json = ""
    return geo_json

if __name__ == "__main__":
    try:
        path = sys.argv[1]
    except:
        print("usage: python firebase2geojson.py teste-bb0d8-export.json > out.json")
        exit()
    
    with open(path) as json_file:
        json = json_file.read()
    
    print(convert(json))

# ref.: https://gist.github.com/sgillies/1233327#file-geojson-spec-1-0-L96