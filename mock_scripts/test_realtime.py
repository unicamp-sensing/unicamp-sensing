from firebase import firebase
import time
firebase = firebase.FirebaseApplication('https://teste-bb0d8.firebaseio.com', None)


lon = -47.06103

for i in range (0, 20):
    # time.sleep(5)
    path = '/2021/11/14/17/43/'
    path = path + str(i)
    path = path + '/A020A61910AC'
    lon += 0.00009
    print(path)
    result = firebase.put(path, name='alt', data='500')
    result = firebase.put(path, name='hdop', data='0.9')
    result = firebase.put(path, name='hum', data='58')
    result = firebase.put(path, name='lat', data='-22.81544')
    result = firebase.put(path, name='lon', data=str(lon))
    result = firebase.put(path, name='pm10', data='0.01')
    result = firebase.put(path, name='pm25', data='0.01')
    result = firebase.put(path, name='pm25', data='0.01')
    result = firebase.put(path, name='tmp', data='27')
    result = firebase.put(path, name='vdop', data='1.4')
    result = firebase.put(path, name='vel', data='0')
