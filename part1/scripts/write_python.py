import serial
import time
ser = serial.Serial('/dev/ttyACM0', 9600)
 
data_to_send = "Hello World " 

while True :
    data = ser.read()
    print(data)
