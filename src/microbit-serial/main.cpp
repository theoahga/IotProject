#include "MicroBit.h"
#include "bme280.h"
#include "ssd1306.h"
#include <stdio.h>
#include <string.h>
#include <cctype>

#define MAX_TEXT_LENGTH 20

ManagedString CODE = "DMST";


MicroBit uBit;
MicroBitI2C i2c(I2C_SDA0, I2C_SCL0);
MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_DIGITAL_OUT);
MicroBitSerial serial(USBTX, USBRX);


char encryptedText[MAX_TEXT_LENGTH];
char decryptedText[MAX_TEXT_LENGTH];

int key = 18;

char* encrypt(char *text, int key) {
    int textLength = strlen(text);

    for (int i = 0; i < textLength-1; i++) {
        text[i] = text[i] + key;
    }
    return text;
}

char* decrypt(char *text, int key) {
    int textLength = strlen(text);
    for (int i = 0; i < textLength; i++) {
        text[i] = text[i] - key;
    }
    return text;
}

char* decode_RF(ManagedString s){
    return decrypt(const_cast<char*>(s.substring(sizeof(CODE), s.length()).toCharArray()), key);
}

void send_RF(ManagedString s){
    ManagedString tosend = CODE + s;
    uBit.radio.datagram.send(tosend);
}

void send_encrypt_RF(ManagedString s){
    char* encryptedText = encrypt(const_cast<char*>(s.toCharArray()), key);
    send_RF(encryptedText);
}


bool check_dmst(ManagedString s) {
    return s.substring(0, sizeof(CODE)) == CODE;
}

void onData(MicroBitEvent) {
    // received data from microbit radio
    ManagedString recData = uBit.radio.datagram.recv();

    if (check_dmst(recData)) {
        char* decryptedText = decode_RF(recData);
        serial.send(decryptedText);
        serial.send("\n");
    }
}
// void onSerialData() {
//     uBit.display.scroll("0");
//     ManagedString receivedData = "1";
//     receivedData = uBit.serial.read();
//
//     uBit.display.scroll(receivedData);
// }
 

int main() {

    // Initialize the micro:bit runtime.
    uBit.init();
    uBit.serial.baud(115200);
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    // uBit.messageBus.listen(MICROBIT_ID_SERIAL, MICROBIT_SERIAL_EVT_RX_FULL, onSerialData);

    uBit.radio.enable();
    uBit.radio.setGroup(57);

    int t = 150;
    
    while (1) {

        if (uBit.buttonA.isPressed()) {
            // serial.send;
            char tosend[128];
            snprintf(tosend, sizeof(tosend), "T:99.99;L:999;%d|", t);
            serial.send(encrypt(tosend, key));
            t++;
        }
        // serial.send("SWITCH");
        ManagedString toRead = serial.read(sizeof(char[128]), ASYNC);

        // onSerialData();
        // serial.send("READ:");

        // receive data from serial : LT or TL
        if (sizeof(toRead) != 0 && (toRead == "LT" || toRead == "TL"))
        {
            //TODO serial.send and scroll is just for debugging => remove it
            uBit.display.scroll(toRead);
            serial.send(ManagedString(toRead + "|"));
            // send it encrypted to distant microbit
            // send_encrypt_RF(toRead);
        }

        uBit.sleep(1000);
    }
}
