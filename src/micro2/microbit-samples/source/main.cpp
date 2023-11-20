
#include "MicroBit.h"
#include "ssd1306.h"
#include "bme280.h"
#include "tsl256x.h"
#include <stdio.h>
#include <string.h>
#include <cctype>

MicroBit uBit;
MicroBitI2C i2c(I2C_SDA0,I2C_SCL0);
MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_DIGITAL_OUT);
ssd1306 screen(&uBit, &i2c, &P0);

ManagedString CODE = "DMST:";
int key = 18;

char ORDER[] = "LT";

#define MAX_TEXT_LENGTH 20

char encryptedText[MAX_TEXT_LENGTH];
char decryptedText[MAX_TEXT_LENGTH];

//Encryption des données RF
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


// Vérification que la trame est bien pour nous
bool check_dsmt(ManagedString s){
    return s.substring(0,CODE.length()) == CODE;
}

//Envoie des données en RF
void send_RF(ManagedString s){
    ManagedString tosend = CODE + s;
    uBit.radio.datagram.send(tosend);
}

//Encryption des données
void send_encrypt_RF(ManagedString s){
    char* encryptedText = encrypt(const_cast<char*>(s.toCharArray()), key);
    send_RF(encryptedText);
}

//Décodage des données reçu
ManagedString decode_RF(ManagedString s){
    return decrypt(const_cast<char*>(s.substring(CODE.length(), s.length()-1).toCharArray()), key);
}

//Quand on reçoit des données en RF
void onData(MicroBitEvent)
{
    ManagedString s = uBit.radio.datagram.recv();

    if (check_dsmt(s)){
        strncpy(ORDER, decode_RF(s).toCharArray(), sizeof(ORDER)-1); //On modifie l'ordre
    }
}

//Récupéation des données
void data(bme280 *bme,tsl256x *tsl, ManagedString order ){

    uint32_t pressure = 0;
    int32_t temp = 0;
    uint16_t humidite = 0;
    
    uint16_t comb =0;
    uint16_t ir = 0;
    uint32_t lux = 0;

    bme->sensor_read(&pressure, &temp, &humidite);
    short tmp = bme->compensate_temperature(temp);
    short pres = bme->compensate_pressure(pressure)/100;
    short hum = bme->compensate_humidity(humidite);

    tsl->sensor_read(&comb, &ir, &lux);
    
    //Création de la trame RF
    ManagedString d = "T:"+ ManagedString(tmp/100) + "." + (tmp > 0 ? ManagedString(tmp%100): ManagedString((-tmp)%100))+";L:"+ ManagedString((int)lux) +"|";
    
    short datalignestart = 0;
    
    //Affichage des données celon l'ordre
    for (int i=0; i<order.length(); i++){
        screen.display_line(i + datalignestart,0, "              ");
        switch(order.toCharArray()[i]) {
        case 'T':
            screen.display_line(i + datalignestart,0, tempdys.toCharArray());
            break;
        case 'H':
            screen.display_line(i + datalignestart,0, humdys.toCharArray());
            break;
        case 'P':
            screen.display_line(i + datalignestart,0, presdys.toCharArray());
            break;
        case 'L':
            screen.display_line(i + datalignestart,0, luxdys.toCharArray());
            break;
        default:
            break;
        }
    }

    //On envoit les données
    send_encrypt_RF(d);
    screen.update_screen();
}



int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    uBit.radio.setGroup(57);
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();

    screen.display_line(0,0,"Hello");
    screen.update_screen();

    bme280 bme(&uBit,&i2c);
    tsl256x tsl(&uBit,&i2c);
    
    
    while(1)
    {
        data(&bme, &tsl, ORDER);
        uBit.sleep(1000);
    }

    release_fiber();

}

