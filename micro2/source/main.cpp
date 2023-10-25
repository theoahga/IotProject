
#include "MicroBit.h"
#include "ssd1306.h"
#include "bme280.h"
#include "tsl256x.h"

MicroBit uBit;
MicroBitI2C i2c(I2C_SDA0,I2C_SCL0);
MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_DIGITAL_OUT);
ssd1306 screen(&uBit, &i2c, &P0);
MicroBitSerial serial(USBTX, USBRX);

ManagedString CODE = "DMST";
ManagedString ESCAPE = "|";

ManagedString ORDER = "LPHT";
int datalignestart = 4;

bool check_dsmt(ManagedString s){
    return s.substring(0,sizeof(CODE)) == CODE;
}

void send_RF(ManagedString s){
    ManagedString tosend = CODE + s;
    uBit.radio.datagram.send(tosend);
    screen.display_line(3,0,tosend.toCharArray());
    screen.update_screen();
}

ManagedString decode_RF(ManagedString s){
    return s.substring(sizeof(CODE), s.length());
}

void onData(MicroBitEvent)
{
    ManagedString s = uBit.radio.datagram.recv();
    screen.display_line(1,0,s.toCharArray());
    
    screen.display_line(3,0,s.substring(0,sizeof(CODE)).toCharArray());

    screen.update_screen();

    if (s == "1")
        uBit.display.print("A");

    if (s == "2")
        uBit.display.print("B");

    if (check_dsmt(s)){
        screen.display_line(2,0,decode_RF(s).toCharArray());
        screen.update_screen();
    }
    

    printf(s.toCharArray());
    serial.send("\n");

}

void data(bme280 *bme,tsl256x *tsl, ManagedString order ){
    uint32_t pressure = 0;
    int32_t temp = 0;
    uint16_t humidite = 0;
    
    uint16_t comb =0;
    uint16_t ir = 0;
    uint32_t lux = 0;

    bme->sensor_read(&pressure, &temp, &humidite);
    int tmp = bme->compensate_temperature(temp);
    int pres = bme->compensate_pressure(pressure)/100;
    int hum = bme->compensate_humidity(humidite);

    tsl->sensor_read(&comb, &ir, &lux);


    ManagedString tempdys = "Temp:" + ManagedString(tmp/100) + "." + (tmp > 0 ? ManagedString(tmp%100): ManagedString((-tmp)%100))+" C";
    ManagedString humdys = "Humi:" + ManagedString(hum/100) + "." + ManagedString(tmp%100)+" rH";
    ManagedString presdys = "Pres:" + ManagedString(pres)+" hPa";
    ManagedString luxdys = "Lux:" + ManagedString((int)lux);
    
    screen.display_line(3,0, order.toCharArray());
    
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
        if (uBit.buttonA.isPressed())
        {

            send_RF("Coucou");
            serial.send("test");
        }

        else if (uBit.buttonB.isPressed())
        {

            send_RF("Hello World ! ");

        }
        data(&bme, &tsl, ORDER);
        
        
        
        uBit.sleep(100);
    }

    release_fiber();

}

