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

HOST           = "0.0.0.0"
UDP_PORT       = 10000
MICRO_COMMANDS = ["TL" , "LT"]
FILENAME        = "values.txt"
LAST_VALUE      = ""
HEADER = "DMST:"
KEY = 12341


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
    with open("values.txt", "r") as f:
        lines = f.readlines()
    if lines:
        lastline = lines[-1]
        f.close()
        return lastline
    else:
        return None 
    
def SendMessageToAndroid(message, ip, port):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(bytes(message,'UTF-8'), (ip, port))


class ThreadedUDPRequestHandler(socketserver.BaseRequestHandler):
        def checkHeader(self ,data):
                message = str(data,"UTF-8")
                header = HEADER
                if message.startswith(header):
                        return message[len(header):]
                else:
                        return ""

        def handle(self):
                data = self.request[0].strip()
                socket = self.request[1]
                current_thread = threading.current_thread()
                message = self.checkHeader(data)
                if message != None:
                        decrypted_text = decrypt(message, KEY)
                        print("{}: client: {}, wrote: {}".format(current_thread.name, self.client_address, decrypted_text))
                        if decrypted_text != "":
                                        if decrypted_text in MICRO_COMMANDS: 
                                                sendUARTMessage(HEADER + encrypt(decrypted_text,KEY))
                                        elif decrypted_text == "getValues()": 
                                                messageToSend = HEADER + encrypt(readLastValue(), KEY)
                                                SendMessageToAndroid(messageToSend, self.client_address[0], 10000)
                                        else:
                                                print("Unknown message: ",decrypted_text)







class ThreadedUDPServer(socketserver.ThreadingMixIn, socketserver.UDPServer):
    pass


# send serial message 
SERIALPORT = "COM3"
BAUDRATE = 115200
ser = serial.Serial()

def initUART():        
        # ser = serial.Serial(SERIALPORT, BAUDRATE)
        ser.port=SERIALPORT
        ser.baudrate=BAUDRATE
        ser.bytesize = serial.EIGHTBITS #number of bits per bytes
        ser.parity = serial.PARITY_NONE #set parity check: no parity
        ser.stopbits = serial.STOPBITS_ONE #number of stop bits
        ser.timeout = None          #block read

        # ser.timeout = 0             #non-block read
        # ser.timeout = 2              #timeout block read
        ser.xonxoff = False     #disable software flow control
        ser.rtscts = False     #disable hardware (RTS/CTS) flow control
        ser.dsrdtr = False       #disable hardware (DSR/DTR) flow control
        #ser.writeTimeout = 0     #timeout for write
        print("Starting Up Serial Monitor")
        try:
                ser.open()
        except serial.SerialException:
                print("Serial {} port not available".format(SERIALPORT))
                exit()



def sendUARTMessage(msg):
    ser.write(msg)
    print("Message <" + msg + "> sent to micro-controller." )


# Main program logic follows:
if __name__ == '__main__':
        #initUART()
        f= open(FILENAME,"a")
        print ('Press Ctrl-C to quit.')

        server = ThreadedUDPServer((HOST, UDP_PORT), ThreadedUDPRequestHandler)

        server_thread = threading.Thread(target=server.serve_forever)
        server_thread.daemon = True

        try:
                server_thread.start()
                print("Server started at {} port {}".format(HOST, UDP_PORT))
                
                server_thread.join()
        except (KeyboardInterrupt, SystemExit):
                server.shutdown()
                server.server_close()
                f.close()
                ser.close()
                exit()