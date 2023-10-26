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

int key = 12341;

char* encrypt(char *text, int key) {
    int textLength = strlen(text);
    for (int i = 0; i < textLength; i++) {
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
    ManagedString recData = uBit.radio.datagram.recv();

    if (check_dmst(recData)) {
        char* decryptedText = decode_RF(recData);
        serial.send(decryptedText);
        serial.send("\n");
    }
}
 

int main() {

    // Initialize the micro:bit runtime.
    uBit.init();
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();
    uBit.radio.setGroup(57);

    while (1) {

        if (uBit.buttonA.isPressed()) {
            // serial.send("SWITCH");
            // ManagedString toRead = serial.read(sizeof("DMSTTL"), ASYNC);

            // serial.send("READ:");
            // serial.send(toRead);
            // send_encrypt_RF(toRead);
        }
        // else if (uBit.buttonB.isPressed()) {           
        //     // LT
        //     send_encrypt_RF("LT");
        //     serial.send("DMSTLT encrypted send");
        //     // serial.send(encrypt("LT", key));
        // }
            serial.send("SWITCH");
            ManagedString toRead = serial.read(sizeof("DMSTTL"), ASYNC);

            serial.send("READ:");
            serial.send(toRead);
            send_encrypt_RF(toRead);


        uBit.sleep(100);
    }
}

