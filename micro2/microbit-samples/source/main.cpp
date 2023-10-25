

#include "MicroBit.h"
#include "ssd1306.h"

MicroBit uBit;
MicroBitI2C i2c(I2C_SDA0,I2C_SCL0);
MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_DIGITAL_OUT);
ssd1306 screen(&uBit, &i2c, &P0);
MicroBitSerial serial(USBTX, USBRX); 

#define CODE "DMST"
#define ESCAPE "|"

bool check_dsmt(ManagedString s){
    return s.substring(0,sizeof(CODE)) == CODE;
}

void send_RF(ManagedString s){
    uBit.radio.datagram.send(CODE + ESCAPE + s);
    screen.display_line(3,0,"send : " + CODE + ESCAPE + s);
    screen.update_screen();
}

ManagedString decode_RF(ManagedString s){
    return s.substring(sizeof(CODE) + sizeof(ESCAPE), s.length());
}

void onData(MicroBitEvent)
{
    ManagedString s = uBit.radio.datagram.recv();
    screen.display_line(1,0,s.toCharArray());

    if (check_dsmt(s)){
        if (s == "1")
        uBit.display.print("A");

        if (s == "2")
            uBit.display.print("B");
        
        screen.display_line(2,0,decode_RF(s));
        screen.update_screen();

        printf(s.toCharArray());
        serial.send("\n");
    }    

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

    while(1)
    {
        if (uBit.buttonA.isPressed())
        {
            send_RF("2");
        }

        else if (uBit.buttonB.isPressed())
        {
            uBit.radio.datagram.send("2");
            screen.display_line(2,0,"send : 2");
            screen.update_screen();
        }

        uBit.sleep(100);
    }

    release_fiber();

}
