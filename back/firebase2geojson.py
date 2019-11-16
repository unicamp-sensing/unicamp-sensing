import sys
import json
import geojson
from geojson import Feature, Point, FeatureCollection

def board_values(required_keys=['lat', 'lon'], print_errors=False):
    ''' Iterator for the board values that contain all keys on `required_keys`\n
        Returns a list of triplets: (`board_values_dict`, `date_str`, `time_str`) '''
    print(f"required keys: {', '.join(required_keys)}")
    triplets = []
    for year, _year in firebase_json.items():
        for month, _month in _year.items():
            for day, _day in _month.items():
                for hour, _hour in _day.items():
                    try:
                        for minute, _minute in _hour.items():
                            for second, _second in _minute.items():
                                for board_MAC_addr, _board_values in _second.items():
                                    if all(key in _board_values.keys() for key in required_keys):
                                        _board_values['board_MAC_addr'] = board_MAC_addr
                                        triplets.append((_board_values, f"{year}-{month}-{day}", f"{hour}:{minute}:{second}"))
                    except Exception as e:
                        if print_errors:
                            print(f"exception at {year}-{month}-{day} {hour}h:\n{e}")
    return triplets

def triplet2feature(triplet, kept_keys=['hum', 'tmp', 'pm10', 'pm25', 'lat', 'lon', 'board_MAC_addr']):
    ''' Converts a (`board_values_dict`, `date_str`, `time_str`) to a `geojson.Feature` object\n
        The list `kept_keys` defines which values will be stored as properties of the feature '''
    board_values_dict, date_str, time_str = triplet
    
    properties = { k: v for k, v in board_values_dict.items() if k in kept_keys }
    properties.update({ 'time': f"{date_str}T{time_str}Z" }) # FIXME Z is used to signal UTC time (https://www.w3.org/TR/NOTE-datetime)
    
    lat, lon = float(board_values_dict['lat']), float(board_values_dict['lon'])
    # NOTE: GeoJSON coordinates are [lon, lat]
    return Feature(geometry=Point((lon, lat)), properties=properties)

def convert(firebase_json):
    ''' Converts `firebase_json` from a Firebase structured string to a `geojson.FeatureCollection` '''
    feature_list = [triplet2feature(triplet) for triplet in board_values()]
    return FeatureCollection(feature_list)

if __name__ == "__main__":
    try:
        path = sys.argv[1]
        out_path = sys.argv[2]
        indent = None if len(sys.argv) <= 3 else int(sys.argv[3]) # leave as None for a 'minified' output
    except:
        print("usage: python firebase2geojson.py teste-bb0d8-export.json out.json [indent]")
        exit()
    
    with open(path, 'r') as json_file:
        firebase_json = json_file.read()
        firebase_json = json.loads(firebase_json)
    
    with open(out_path, 'w+') as geojson_file:
        geojson.dump(convert(firebase_json), geojson_file, indent=indent)
    
    print("done.")
    