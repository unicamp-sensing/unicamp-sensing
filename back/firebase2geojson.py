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
#       https://geoffboeing.com/2015/10/exporting-python-data-geojson/
#       https://stackoverflow.com/a/47325088
#       https://gis.stackexchange.com/a/221040
#       https://github.com/geopandas/geopandas
#       https://geopandas.readthedocs.io/en/latest/gallery/create_geopandas_from_pandas.html