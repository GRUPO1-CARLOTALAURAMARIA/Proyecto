/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "Grove_LCD_RGB_Backlight.h"
#include "BMP180.h"
#define fmin 100.0
#define fmax 220.0
#define ledPeriod 100000

static UnbufferedSerial serial_port_hc06(PA_0, PA_1, 9600);
static UnbufferedSerial serial_port_usb(USBTX, USBRX, 9600);

char datoRx[1];

Grove_LCD_RGB_Backlight rgbLCD(PB_9,PB_8);


AnalogIn int_temp(ADC_TEMP);
DigitalOut led1(D2);
DigitalOut led2(D3);
DigitalIn boton(D5);
DigitalIn touch(D4);
PwmOut servomotor(A2);
I2C i2c(I2C_SDA, I2C_SCL);
static UnbufferedSerial serial_port(PA_0, PA_1, 9600);

enum estados {apagada,midiendo,encendida,enviardato} estado;
#define WAIT_TIME_MS 100

void estadoapagada  ()
{
   if (boton==1){ //Si se pulsa el boton mide la temnperatura
        estado=midiendo;
        }
    else if (touch==1){
        estado=enviardato;
    }
}

void estadomidiendo  ()
{
    
        float datof=int_temp.read(); //Lee el dato en flotante entre 0 y 1.0. Para 0V 0 y para 3.3V 1.0
        float datoV=int_temp.read_voltage(); //Lee el dato en voltios. De 0 a Vref, 3.3V
        int datoI=int_temp.read_u16(); //Lee el dato en entero como si el ADC fuese de 16bits, aunque no lo sea.
        float temp=((datoV - 0.76)/2.5) + 25;
        rgbLCD.setRGB(0xff, 0xff, 0xff);                 //set the color 
        rgbLCD.locate(0,0);
        rgbLCD.print("Temperatura=");
        char datos[8];
        sprintf(datos,"%f",temp);
        rgbLCD.print(datos);
        rgbLCD.locate(0,1);
        //Ejemplos de leer una señal analógica
        printf("El dato leido entre 0 y 1 es: %f\n",datof);
        printf("El dato leido en voltios entre 0 y 3.3 es: %f\n",datoV);
        printf("El dato leido del ADC de 16bits es: %d\n",datoI);
        printf("La temperatura es: %fCº\n",temp);
        printf("*************************************\n");
        servomotor.period_ms(20);
        int anchoPulso;
        wait_us(1000000);
            if(temp>26)
            {
                led1=1;
                led2=0;
                estado=apagada;
            }
            else 
                {
                 estado=encendida;
                 led1=0;
                 led2=1;
                }
            if(temp<=15)
            {
             anchoPulso=0;
             servomotor.pulsewidth_us(anchoPulso);
            }
            else if(15<temp<=20) {
                anchoPulso=1000;
                servomotor.pulsewidth_us(anchoPulso);

            }
            else if(20<temp<=25) {
                anchoPulso=2000;
                servomotor.pulsewidth_us(anchoPulso);

            }
            else {
                anchoPulso=3000;
                servomotor.pulsewidth_us(anchoPulso);

            }
}

void estadoenviardato()
{
    if(boton==1){
        estado=midiendo;
    }
    servomotor.period_ms(20);
        int anchoPulso;
        wait_us(1000000);
    if (serial_port.readable() == true) {
    float temp2=serial_port.read(&datoRx, 1);

  if(temp2<=15)
    {
    anchoPulso=0;
    servomotor.pulsewidth_us(anchoPulso);
    }
    else if(15<temp2<=20) 
    {
        anchoPulso=1000;
        servomotor.pulsewidth_us(anchoPulso);
    }
    else if(20<temp2<=25)
    {
        anchoPulso=2000;
        servomotor.pulsewidth_us(anchoPulso);
    }
    else 
    {
        anchoPulso=3000;
        servomotor.pulsewidth_us(anchoPulso);

    }
}
}


void estadoencendida ()
{
    if (boton==1)
    {
        estado=midiendo;
    }
   

}
int main()
{
    serial_port_hc06.set_blocking(false);
    serial_port_usb.set_blocking(false);
    //Establecer la tensión de referencia
    int_temp.set_reference_voltage(3.3);
    estado=apagada;
    led1=1;
    led2=0;
    while (true)
    {
         switch(estado) 
        {
            case apagada:
                estadoapagada();
                break;
            case encendida:
                estadoencendida();
                break;
            case midiendo:
                estadomidiendo();
                break;
            case enviardato:
                estadoenviardato();
                break;
        }

    if (serial_port_hc06.readable() == true) {
    serial_port_hc06.read(&datoRx, 1);
    serial_port_usb.write(&datoRx, 1);
    }
    if (serial_port_usb.readable() == true) {
    serial_port_usb.read(&datoRx, 1);
    serial_port_hc06.write(&datoRx, 1);
    }

    wait_us(1000);     
        
       
    }
}