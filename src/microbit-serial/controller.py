# Program to control passerelle between Android application
# and micro-controller through USB tty
import time
import argparse
import signal
import sys
import socket
import socketserver
import serial
import threading
from datetime import datetime
import mysql.connector

from database_manager import DatabaseManager

HOST = "0.0.0.0"
UDP_PORT = 10000
MICRO_COMMANDS = ["TL", "LT"]
FILENAME = "values.txt"
LAST_VALUE = ""
HEADER = "DMST:"
KEY = 18

ORDER = "TL"

def encrypt(text, key):
    encrypted_text = ''
    for char in text:
        encrypted_char = chr(ord(char) + key)
        encrypted_text += encrypted_char
    return encrypted_text

def decrypt(text, key):
    decrypted_text = ''
    for char in text:
        decrypted_char = chr(ord(char) - key)
        decrypted_text += decrypted_char
    return decrypted_text

class ThreadedUDPRequestHandler(socketserver.BaseRequestHandler):
        
    db = DatabaseManager()
    db.connect()

    def handle(self):
        # Get UDP message from android app
        data = self.request[0].strip()
        # Get socket to send message back to android app
        socket = self.request[1]
        current_thread = threading.current_thread()
        print("{}: client: {}, wrote: {}".format(current_thread.name, self.client_address, data))
        

        # Send message if data is not empty
        if data != "":
            # if data is TL or LT
            if data in MICRO_COMMANDS:  # Send message to microbit through UART, to change order
                sendUARTMessage(data)

            elif data == "getValues()":  # Request from android app to get last values
                last_temp, last_lux, last_time = db.get_last_value()
                socket.sendto(LAST_VALUE, self.client_address)
            else:
                print("Unknown message: ", data)


class ThreadedUDPServer(socketserver.ThreadingMixIn, socketserver.UDPServer):
    pass


# send serial message 
SERIALPORT = "/dev/ttyACM0"
BAUDRATE = 115200
ser = serial.Serial()


def initUART():
    # ser = serial.Serial(SERIALPORT, BAUDRATE)
    ser.port = SERIALPORT
    ser.baudrate = BAUDRATE
    ser.bytesize = serial.EIGHTBITS  # number of bits per bytes
    ser.parity = serial.PARITY_NONE  # set parity check: no parity
    ser.stopbits = serial.STOPBITS_ONE  # number of stop bits
    ser.timeout = None  # block read

    # ser.timeout = 0             #non-block read
    # ser.timeout = 2              #timeout block read
    ser.xonxoff = False  # disable software flow control
    ser.rtscts = False  # disable hardware (RTS/CTS) flow control
    ser.dsrdtr = False  # disable hardware (DSR/DTR) flow control
    # ser.writeTimeout = 0     # timeout for write
    print('Starting Up Serial Monitor')
    try:
        ser.open()
    except serial.SerialException:
        print("Serial {} port not available".format(SERIALPORT))
        exit()


def sendUARTMessage(msg):
    ser.write(bytes(msg, 'UTF-8'))
    print("Message <" + msg + "> sent to micro-controller.")

def extractDataFromSerial(data):
    # Exemple de chaîne de données
    # T:23.45;L:500;1635486321
    parts = data.split(';')

    temperature = float(parts[0].split(':')[1])
    lux = float(parts[1].split(':')[1])
    timestamp = int(parts[2])

    return temperature, lux, timestamp

# Main program logic follows:
if __name__ == '__main__':
    # Init UART connection to microbit
    initUART()
    # Init database connection
    db = DatabaseManager()
    db.connect()
    # Open file to write values
    f = open(FILENAME, "a")
    print('Press Ctrl-C to quit.')
    # Create UDP server to communicate with android app
    server = ThreadedUDPServer((HOST, UDP_PORT), ThreadedUDPRequestHandler)
    
    server_thread = threading.Thread(target=server.serve_forever)
    server_thread.daemon = True

    delimiter = b"|"
    buffer = b""
    message_str = ""
    
    try:
        # Start a thread with the server
        server_thread.start()
        print("Server started at {} port {}".format(HOST, UDP_PORT))
        while ser.isOpen():
            # time.sleep(100)
            if (ser.inWaiting() > 0):  # if incoming bytes are waiting
                # Read incoming data from Microbit
                data_new = ser.read(ser.inWaiting())
                # We use a buffer to store the data until we find the delimiter
                buffer += data_new
                # print("Buffer after reading data:", buffer)
                while delimiter in buffer:
                    message, buffer = buffer.split(delimiter, 1)
                    message_str_encoded = message.decode('utf-8')
                    message_str_decoded = decrypt(message_str_encoded, KEY) 
                    
                    print("RECEIVED FROM SERIAL (encoded): " + message_str_encoded)
                    print("RECEIVED FROM SERIAL (decoded): " + message_str_decoded + "\n")

                    if ("SWITCH" in message_str):
                       sendUARTMessage(ORDER)
                    else:
                        # Extract data
                        if message_str_decoded not in MICRO_COMMANDS:
                            temp, lux, time = extractDataFromSerial(message_str_decoded)
                            # Insert data in database
                            if db.insert_data(temp, lux, datetime.now()):
                                print("Data inserted in database :", temp, lux, datetime.now())
                            else:
                                print("Error while inserting data in database")
                        # f.write(message_str)
                        # LAST_VALUE = message_str
                        # print("time :" + datetime.datetime.now());


    except (KeyboardInterrupt, SystemExit):
        db.print_all_values()
        db.close()
        server.shutdown()
        server.server_close()
        f.close()
        ser.close()
        exit()
