/*Desarrolle un programa que reproduzca de forma cíclica dos o más secuencias de colores utilizando el LED
RGB integrado en la placa.
Cada secuencia debe estar compuesta por tres colores y debe repetirse tres veces antes de pasar a la
siguiente. El retardo entre los colores de una misma secuencia debe ser corto y constante.
• Ejemplo:
o secuencia_A: rojo -> verde -> amarillo.
o secuencia_B: azul -> magenta -> blanco.
o secuencia_A -> secuencia_B -> secuencia_A -> secuencia_B -> ...*/

#include "LPC17xx.h"

#define PORT3_25 (1<<25)
#define PORT3_26 (1<<26)
#define PORTS_SEC_A (7 << 22);

volatile const uint32_t delay_sec = 6000000000;
volatile const uint32_t delay_led = 3000000000;

void configSecA ();
void configSecB ();
void delay(uint32_t);
void secA(uint32_t);
void secA (uint32_t);

int main (){
	configSecA();
	configSecB();
	while(1){

		secA(delay_led);
		delay(delay_sec);
		secB(delay_led);
		delay(delay_sec);
	}
	return 0 ;
};
void configSecA (){
	//Pines en modo GPIO
	LPC_PINCON ->PINSEL0 &= ~ (3<<12);
	LPC_PINCON ->PINSEL0 &= ~ (3<<14);
	LPC_PINCON ->PINSEL0 &= ~ (3<<16);
	//Pines activos por alto
	LPC_PINCON ->PINMODE1 |= (3<<12);
	LPC_PINCON ->PINMODE1 |= (3<<14);
	LPC_PINCON ->PINMODE1 |= (3<<16);
	//Pines definidos como salidas
	LPC_GPIO0 ->FIODIR |= PORTS_SEC_A;
	LPC_GPIO0 ->FIOMASK &= ~PORTS_SEC_A;
	LPC_GPIO0 ->FIOCLR |= PORTS_SEC_A;
}

void configSecB(){
	LPC_PINCON ->PINSEL0 &= ~ (3<<17);
	LPC_PINCON->PINSEL7 &= ~(3<<18); // P3_25 MODO GPIO
	LPC_PINCON->PINSEL7 &= ~(3<<20); // P3_26 MODO GPIO

	LPC_PINCON ->PINMODE1 |= (3<<18);
	LPC_PINCON ->PINMODE7 |= (3<<18);
	LPC_PINCON ->PINMODE7 |= (3<<20);

	LPC_GPIO0->FIODIR |= (1<<25);
	LPC_GPIO0 ->FIOMASK &= ~(1<< 25);
	LPC_GPIO0 ->FIOCLR |= (1 << 25);

	LPC_GPIO3 -> FIODIR |= PORT3_25;
	LPC_GPIO3 -> FIOMASK &= ~ PORT3_25;
	LPC_GPIO3 -> FIOCLR |= PORT3_25;

	LPC_GPIO3 -> FIODIR |= PORT3_26;
	LPC_GPIO3 -> FIOMASK &= ~ PORT3_26;
	LPC_GPIO3 -> FIOCLR |= PORT3_26;
}
void delay (uint32_t param){
	for (volatile uint32_t i = 0; i< param ; i++){
		__NOP();
	}
}
void SecA (uint32_t d){
	LPC_GPIO0 ->FIOSET |=(1<<25);
	delay(d);
	LPC_GPIO0 ->FIOCLR |= (1<<22);
	delay(d);
	LPC_GPIO0 ->FIOSET |=(1<<23);
	delay(d);
	LPC_GPIO0 ->FIOCLR |= (1<<23);
	delay(d);
	LPC_GPIO0 ->FIOSET |=(1<<24);
	delay(d);
	LPC_GPIO0 ->FIOCLR |= (1<<24);
	delay(d);
}

void secB(uint32_t d){
	LPC_GPIO0 ->FIOSET |=(1<<25);
	delay(d);
	LPC_GPIO0 ->FIOCLR |= (1<<25);
	delay(d);
	LPC_GPIO3 ->FIOSET |=(PORT3_25);
	delay(d);
	LPC_GPIO3 ->FIOCLR |= PORT3_25;
	delay(d);
	LPC_GPIO3 ->FIOSET |= PORT3_26;
	delay(d);
	LPC_GPIO3 ->FIOCLR |= PORT3_26;
	delay(d);
}
