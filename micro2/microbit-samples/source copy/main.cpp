/*
   The MIT License (MIT)

   Copyright (c) 2016 British Broadcasting Corporation.
   This software is provided by Lancaster University by arrangement with the BBC.

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
   */

#include "MicroBit.h"

#include "inc/neopixel.h"

#include "tsl256x.h"

#include "veml6070.h"
#include "ssd1306.h"

#define led_pin MICROBIT_PIN_P1
#define led_count 2

MicroBit uBit;

MicroBitSerial serial(USBTX, USBRX); 
MicroBitI2C i2c(I2C_SDA0,I2C_SCL0);
MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_DIGITAL_OUT);

void temperature_test() {
    int temp = uBit.thermometer.getTemperature();
    uBit.display.scroll("temp: " + ManagedString(temp) + "C");
    if (temp >=30){
        uBit.display.scroll("Alerte !");
    }
    
}

void
accelerometer_test1()
{
    int n=0;
    while(1)
    {
        int x = uBit.accelerometer.getX();
        if (x< -500){
            n++;
            uBit.sleep(1000);
        }
        uBit.display.print(n);
    }
}


void compass(){
    int coord[][2]={{1, 0},{0, 0},{0, 1},{0, 2},{0, 3},{0, 4},{1, 4},{2, 4},{3, 4},{4, 4},{4, 3},{4, 2},{4, 1},{4, 0},{3, 0},{2, 0}};

    while(1){
        int v = 15 * uBit.compass.heading() / 365;
        //uBit.display.print(v);
        uBit.display.image.clear();
        uBit.display.image.setPixelValue(coord[v][0], coord[v][1], 255);
        uBit.sleep(100);
    }
}

void feux(){
    uBit.display.disable();
    while (1){
        uBit.io.P7.setDigitalValue(1);
        uBit.sleep(1000);
        uBit.io.P7.setDigitalValue(0);
        uBit.io.P8.setDigitalValue(1);
        uBit.sleep(500);
        uBit.io.P8.setDigitalValue(0);
        uBit.io.P9.setDigitalValue(1);
        uBit.sleep(1000);
        uBit.io.P9.setDigitalValue(0);

    }
}

void led(){
    uBit.display.disable();
    Neopixel neo(led_pin, led_count);
    while(1){
        neo.showColor(0, 0,0,255);
        neo.showColor(1, 255,0,0);
        uBit.sleep(250);
        neo.showColor(0, 255,255,255);
        neo.showColor(1, 255,255,255);
        uBit.sleep(250);
        neo.showColor(0, 255,0,0);
        neo.showColor(1, 0,0,255);
        uBit.sleep(250);

    }
}

void serie(){
    for (int i =0; i<10; i++){
        serial.send(i);
        serial.send("toto\n");
    }
}

void meteo(){
    tsl256x tsl(&uBit,&i2c);
    uint16_t comb =0;
    uint16_t ir = 0;
    uint32_t lux = 0;
    tsl.sensor_read(&comb, &ir, &lux);
    ManagedString display = "Lux:" + ManagedString((int)lux);
    uBit.display.scroll(display.toCharArray());
    serial.send(comb);
    serial.sendChar('\n');
    uBit.sleep(1000);
    
}

void uv(){
    veml6070 veml(&uBit,&i2c);
    uint16_t uv = 0;
    veml.sensor_read(&uv);
    ManagedString display = "UV:" + ManagedString(uv);
    uBit.display.scroll(display.toCharArray());

    uBit.sleep(1000);

} 

void screen(){
    ssd1306 screen(&uBit, &i2c, &P0);
    while(true)
    {
        screen.display_line(0,0,"ABCDEFGHIJKLMNOP");
        screen.display_line(1,0,"BCDEFGHIJKLMNOP");
        screen.display_line(2,0,"CDEFGHIJKLMNOP");
        screen.display_line(3,0,"DEFGHIJKLMNOP");
        screen.display_line(4,0,"EFGHIJKLMNOP");
        screen.display_line(5,0,"FGHIJKLMNOP");
        screen.display_line(6,0,"GHIJKLMNOP");
        screen.display_line(7,0,"HIJKLMNOP");
        screen.update_screen();
        uBit.sleep(1000);
    }
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    screen();
    //0x3D

    release_fiber();

}