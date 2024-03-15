import serial
import csv
import time

    

def main():
    port = "/dev/ttyACM0"
    csv_filename = "data.csv"
    msg = ""

    try:
        ser = serial.Serial(port, 9600, timeout=1)
        print("Serial port opened successfully!")
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        return

    file = open(csv_filename, 'w')
    duty_cycle = 0.0
    timestamp = 0.0
    lux = 0.0



    try:
        while True:
            writer = csv.writer(file)
            data = ser.read().decode('utf-8')

            if data == '\n':
                if 'duty_cycle' in msg:
                    duty_cycle = msg.split(' ')[1]
                    duty_cycle  = duty_cycle.replace('\r', '')
                elif 'lux' in msg:
                    lux = msg.split(':')[1]
                    timestamp += 25 
                    lux = lux.replace('\r', '')

                    print(f"Timestamp: {timestamp}, Duty Cycle: {duty_cycle}, Lux: {lux}")

                    writer.writerow([timestamp, duty_cycle, lux])

                msg = ""
            else :
                msg +=  data




    except KeyboardInterrupt:
        print("Exiting...")
        file.close()
        ser.close()
        return


if __name__ == "__main__":
    main()

