#include "MicroBit.h"
#include "bme280.h"
#include "ssd1306.h"
#include <stdio.h>
#include <string.h>
#include <cctype>

ManagedString CODE = "DMST";


MicroBit uBit;
MicroBitI2C i2c(I2C_SDA0, I2C_SCL0);
MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_DIGITAL_OUT);
MicroBitSerial serial(USBTX, USBRX);

#define MAX_TEXT_LENGTH 20

char encryptedText[MAX_TEXT_LENGTH];
char decryptedText[MAX_TEXT_LENGTH];

char key[] = "bcdefghijklmnopqrstuvwxyz";


char* encrypt(const char* text, char* key) {
    int textLength = strlen(text);

    for (int i = 0; i < textLength; i++) {
        char currentChar = text[i];
        if (isalpha(currentChar)) {
            char isUpperCase = isupper(currentChar);
            currentChar = tolower(currentChar);
            int charIndex = currentChar - 'a';
            char encryptedChar = isUpperCase ? toupper(key[charIndex]) : key[charIndex];
            encryptedText[i] = encryptedChar;
        } else {
            encryptedText[i] = currentChar;
        }
    }
    encryptedText[textLength] = '\0';
    return encryptedText;
}

char* decrypt(const char* encryptedText, char* key) {
    int textLength = strlen(encryptedText);

    for (int i = 0; i < textLength; i++) {
        char currentChar = encryptedText[i];
        if (isalpha(currentChar)) {
            char isUpperCase = isupper(currentChar);
            currentChar = tolower(currentChar);
            char* charIndex = strchr(key, currentChar);
            int index = charIndex - key;
            char decryptedChar = index + 'a';
            decryptedChar = isUpperCase ? toupper(decryptedChar) : decryptedChar;
            decryptedText[i] = decryptedChar;
        } else {
            decryptedText[i] = currentChar;
        }
    }
    decryptedText[textLength] = '\0';
    return decryptedText;
}

ManagedString decode_RF(ManagedString s){
    return s.substring(sizeof(CODE), s.length());
}

void send_RF(ManagedString s){
    ManagedString tosend = CODE + s;
    uBit.radio.datagram.send(tosend);
    screen.display_line(3,0,tosend.toCharArray());
    screen.update_screen();
}

void send_encrypt_RF(ManagedString s){
    char* encryptedText = encrypt(const_cast<char*>(s.toCharArray()), key);

    send_RF(encryptedText);
    
    screen.display_line(1,0,encryptedText);

    char* decryptedText = decrypt(encryptedText, key);

    screen.display_line(2,0,decryptedText);

    screen.update_screen();
}

bool check_dmst(ManagedString s) {
    return s.substring(0, sizeof(CODE)) == CODE;
}

void onData(MicroBitEvent) {
    char data[20];
    ManagedString recData = uBit.radio.datagram.recv();

    // int totalLength = recData.length();
    snprintf(data, 20, "%s\n", recData.toCharArray());

    if (check_dmst(recData)) {
        char* decryptedText = decrypt(decode_RF(recData).toCharArray(), key);
        serial.send(decryptedText);
        serial.send("\n");
    }

    // char* test = serial.read(data, 20, ASYNC);
}
 

int main() {

    // Initialize the micro:bit runtime.
    uBit.init();
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();
    uBit.radio.setGroup(57);

    while (1) {

        if (uBit.buttonA.isPressed()) {
            // TL
            send_encrypt_RF("TL");
            serial.send("TL encrypted send");
            serial.send(encrypt("TL", key));
        }
        else if (uBit.buttonB.isPressed()) {           
            // LT
            send_encrypt_RF("LT");
            serial.send("LT encrypted send");
            serial.send(encrypt("LT", key));

        }

        uBit.sleep(100);
    }
}

