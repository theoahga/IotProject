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
HEADER = "DMST"

class ThreadedUDPRequestHandler(socketserver.BaseRequestHandler):
        def checkHeader(self ,data):
                message = str(data,"UTF-8")
                header = HEADER+":"
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
                        print("{}: client: {}, wrote: {}".format(current_thread.name, self.client_address, message))
                        if message != "":
                                        if message in MICRO_COMMANDS: # Send message through UART
                                                sendUARTMessage(message)
                                                
                                        elif message == "getValues()": # Sent last value received from micro-controller
                                                print("getvalues(): ", message)
                                                #socket.sendto(LAST_VALUE, self.client_address) 
                                                # TODO: Create last_values_received as global variable      
                                        else:
                                                print("Unknown message: ",message)
                else :
                        print("Réception d'un message qui n'est pas nous ")






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