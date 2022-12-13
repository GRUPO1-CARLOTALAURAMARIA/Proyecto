/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "Grove_LCD_RGB_Backlight.h"
#define fmin 100.0
#define fmax 220.0
#define ledPeriod 100000

Grove_LCD_RGB_Backlight rgbLCD(PB_9,PB_8);

AnalogIn int_temp(ADC_TEMP);
DigitalOut led1(D2);
DigitalIn boton(D5);
enum estados {nopulsado,pulsado,encendida} estado;
#define WAIT_TIME_MS 100


void estadonopulsado  ()
{
    if (boton==0)
    estado=pulsado;
}

void estadopulsado ()
{

}
int main()
{
    //Establecer la tensión de referencia
    int_temp.set_reference_voltage(3.3);
    
    while (true)
    {
        if (boton==1){ //Si se pulsa el boton mide la temnperatura

        led1=1;
        float datof=int_temp.read(); //Lee el dato en flotante entre 0 y 1.0. Para 0V 0 y para 3.3V 1.0
        float datoV=int_temp.read_voltage(); //Lee el dato en voltios. De 0 a Vref, 3.3V
        int datoI=int_temp.read_u16(); //Lee el dato en entero como si el ADC fuese de 16bits, aunque no lo sea.
        float temp=((datoV - 0.76)/2.5) + 25;
        
        rgbLCD.setRGB(0xff, 0xff, 0xff);                 //set the color 
        rgbLCD.locate(0,0);
        rgbLCD.print("Temperatura= ");
        rgbLCD.locate(0,1);
        thread_sleep_for(WAIT_TIME_MS);
        //Ejemplos de leer una señal analógica

         printf("El dato leido entre 0 y 1 es: %f\n",datof);
        printf("El dato leido en voltios entre 0 y 3.3 es: %f\n",datoV);
        printf("El dato leido del ADC de 16bits es: %d\n",datoI);
        printf("La temperatura es: %fCº\n",temp);
        printf("*************************************\n");

        thread_sleep_for(WAIT_TIME_MS);
        }
       else {
           led2=1;
       }

       
    }
}