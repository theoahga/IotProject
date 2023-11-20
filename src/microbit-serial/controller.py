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

def readLastValue():
    temp, lux, timestamp = db.get_last_value()
    # convert timestamp to milliseconds
    return "T:{};L:{};{}".format(temp, lux, int(timestamp.timestamp() * 1000))
    
def SendMessageToAndroid(message, ip, port):
        print("Message send : {}, {}, {} ".format(message, ip, port))
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(bytes(message,'UTF-8'), (ip, port))

class ThreadedUDPRequestHandler(socketserver.BaseRequestHandler):
        
    db = DatabaseManager()
    db.connect()

    def checkHeader(self ,data):
        message = str(data,"UTF-8")
        header = HEADER
        if message.startswith(header):
                return message[len(header):]
        else:
                return ""

    def handle(self):
        # Get UDP message from android app
        data = self.request[0].strip()
        # Get socket to send message back to android app
        socket = self.request[1]
        current_thread = threading.current_thread()
        print("{}: client: {}, wrote: {}".format(current_thread.name, self.client_address, data))
        
        message = self.checkHeader(data)
        # Decrypt message if there is one
        if message != None:
            decrypted_text = decrypt(message, KEY)
            print("{}: client: {}, wrote: {}".format(current_thread.name, self.client_address, decrypted_text))
            if decrypted_text != "":
                # If message is TL or LT, send it to microbit through UART, to change order
                if decrypted_text in MICRO_COMMANDS: 
                    sendUARTMessage(decrypted_text)
                # If message is getValues(), send last value to android app
                elif decrypted_text == "getValues()": 
                    messageToSend = HEADER + encrypt(readLastValue(), KEY)
                    SendMessageToAndroid(messageToSend, self.client_address[0], 10001)
                else:
                    print("Unknown message: ",decrypted_text)


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
    try:
        ser.write(msg.encode('utf-8'))
        print("Message <" + msg + "> sent to micro-controller.")
    except serial.SerialException as e:
        print("Error while sending message to micro-controller.")
        print(e)

def extractDataFromSerial(data):
    try:
        # Exemple de chaîne de données
        # T:23.45;L:500
        parts = data.split(';')

        temperature = float(parts[0].split(':')[1])
        lux = float(parts[1].split(':')[1])

        return temperature, lux

    except (ValueError, IndexError) as e:
        print(f"Erreur lors de l'extraction des données: {e}")
        return -1, -1

# Main program logic follows:
if __name__ == '__main__':
    # Init UART connection to microbit
    initUART()
    # Init database connection
    db = DatabaseManager()
    db.connect()
    print('Press Ctrl-C to quit.')
    # Create UDP server to communicate with android app
    server = ThreadedUDPServer((HOST, UDP_PORT), ThreadedUDPRequestHandler)
    server_thread = threading.Thread(target=server.serve_forever)
    server_thread.daemon = True

    delimiter = b"|"
    buffer = b""
    
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
                while delimiter in buffer:
                    message, buffer = buffer.split(delimiter, 1)
                    message_str = message.decode('utf-8')
                    
                    print("(SERIAL) received (encoded): " + message_str)

                    if message_str not in MICRO_COMMANDS:
                        temp, lux = extractDataFromSerial(message_str)
                        # Insert data in database
                        if db.insert_data(temp, lux, datetime.now()):
                            print("Data inserted in database :", temp, lux, datetime.now())
                        else:
                            print("Error while inserting data in database")
        server_thread.join()

    except (KeyboardInterrupt, SystemExit):
        db.print_last_ten_values()
        db.close()
        server.shutdown()
        server.server_close()
        ser.close()
        exit()
