/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "BMP180.h"
#include "Grove_LCD_RGB_Backlight.h"
#include "mbed.h"

#define fmin 100.0
#define fmax 220.0
#define ledPeriod 100000

static UnbufferedSerial serial_port_hc06(PA_0, PA_1, 9600);
static UnbufferedSerial serial_port_usb(USBTX, USBRX, 9600);

char datoRx[1];

Grove_LCD_RGB_Backlight rgbLCD(PB_9, PB_8);

AnalogIn int_temp(ADC_TEMP); // Se define una entrada analógica correspondiente al sensor de la temperatura
DigitalOut led1(D2);         // Se define una sálida digital correspondiente al led rojo
DigitalOut led2(D3);         // Se define una sálida digital correspondeinte al led azul
DigitalIn boton(D5);
DigitalIn botonoff(D4);        // Se define entrada digital correspondiente al botón de activación del sensor de temperatura
PwmOut servomotor(A2);      // Se define la sálida del servomotor
I2C i2c(I2C_SDA, I2C_SCL);  //Se 
static UnbufferedSerial serial_port(PA_0, PA_1, 9600);

enum estados { apagada, midiendo, encendida } estado; //Definimos los diferentes estados, calefacción encendida, midiendo, o apagada
#define WAIT_TIME_MS 100

void estadoapagada() {  //Estado apagada
led1=1;
led2=0;
rgbLCD.setRGB(0xff, 0xff, 0xff); // set the color
rgbLCD.locate(0, 0);
rgbLCD.print("Apagada");
  if (boton == 1) { // Si se pulsa el boton
    estado = midiendo; //Entra en el estado correspondiente a medir la temperatura
  } 
}

void estadomidiendo() { //Estado midiendo
    led1=0;     //El led 1 estará apagado mientrás se toman medidas
    led2=0;     //El led 2 también estará apagado mientrás se toman medidas
  float datof = int_temp.read(); // Lee el dato en flotante entre 0 y 1.0. Para
                                 // 0V 0 y para 3.3V 1.0
  float datoV =
      int_temp.read_voltage();     // Lee el dato en voltios. De 0 a Vref, 3.3V
  int datoI = int_temp.read_u16(); // Lee el dato en entero como si el ADC fuese
                                   // de 16bits, aunque no lo sea.
  float temp = ((datoV - 0.76) / 2.5) + 25; //Se cálcula la temperatura en grados centígrados según la medida del sensor
  rgbLCD.setRGB(0xff, 0xff, 0xff); // set the color
  rgbLCD.locate(0, 0);
  rgbLCD.print("Temperatura="); //Sacamos por pantalla un letrero que muestre Temperatura= 
  char datos[8];
  int l=sprintf(datos, "%f\n", temp);
  rgbLCD.print(datos);
  rgbLCD.locate(0, 1);
  // Ejemplos de leer una señal analógica
  printf("El dato leido entre 0 y 1 es: %f\n", datof);
  printf("El dato leido en voltios entre 0 y 3.3 es: %f\n", datoV);
  printf("El dato leido del ADC de 16bits es: %d\n", datoI);
  printf("La temperatura es: %fCº\n", temp);
  printf("*************************************\n");
  serial_port_hc06.write(datos, l);
  servomotor.period_ms(20);
  int anchoPulso;
  wait_us(1000000);
  if (temp > 26) { //Si la temperatura medida es mayor de 25ºC
    led1 = 1;      //Encendemos el led rojo, que significa que la calefacción se mantendrá apagada.
    led2 = 0;      //El led azul continuará apagado
    anchoPulso = 3000; //Sacamoos el siguiente ancho de pulso por el servo para que situe la aguja en la temperatura correspondiente, en el extremo maximo.
    servomotor.pulsewidth_us(anchoPulso);
    estado = apagada;       //Se entra en el estado de calefacción apagada.
  } else {          //Siempre que la temperatura sea menor de 26ºC se encenderá la calefacción
    led1 = 0;               //Se apaga el led rojo
    led2 = 1;               //Y se enciende el led azul
  
  if (temp <= 15) {     //Si la temperatura es menor de 15ºC
    anchoPulso = 0;     //El ancho del pulso valdrá 0
    servomotor.pulsewidth_us(anchoPulso); //Y la aguja del termostato se posicionará en un extremo
    estado = encendida;     //Entramos en el estado encendida
  } else if (15 < temp <= 20) { //Si la temperatura esta entre 15º y 20ºC
    anchoPulso = 1000;          //El ancho de pulso será de 1000
    servomotor.pulsewidth_us(anchoPulso);  //Para así posicionar la aguja entre un extremo y la mitad
    estado = encendida;     //Entramos en el estado encendida

  } else if (20 < temp <= 25) { //Si la temperatura esta entre 20º y 25ºC
    anchoPulso = 2000;          //El ancho de pulso será de 2000
    servomotor.pulsewidth_us(anchoPulso); //Y se posiconará entre la mitad y el extremo máximo
    estado = encendida;     //Entramos en el estado encendida

  } 
  }
}
void estadoencendida() { //Entramos en estado encendida
led1=0; //La luz roja seguirá apagada
led2=1; // Y la luz azul estará encendida para mostrar que la calefacción está encendida.
  if (boton == 1) { //Si en algun momento se vuelve a pulsar el botón
    estado = midiendo; //Entra de nuevo en el estado de medir
  }
    else if(botonoff == 1) {
            estado=apagada;
        }
    
  
}
int main() {
  serial_port_hc06.set_blocking(false);
  serial_port_usb.set_blocking(false);
  // Establecer la tensión de referencia
  int_temp.set_reference_voltage(3.3);
  estado = apagada; //La calefacción comenzará estando apagda
  led1 = 1; //Por tanto el led rojo estará encendido
  led2 = 0; //Y el led azul apagado
  while (true) { //Definimos el bucle
    switch (estado) { //Y los diferentes estados
    case apagada:
      estadoapagada();
      break;
    case encendida:
      estadoencendida();
      break;
    case midiendo:
      estadomidiendo();
      break;
    }

    wait_us(1000);
  }
}