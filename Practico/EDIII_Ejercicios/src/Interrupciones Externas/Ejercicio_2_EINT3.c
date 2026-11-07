/*
Desarrolle un programa similar al del ejercicio 1.7, pero en esta ocasión, el incremento de la cuenta debe
ser manejado por una interrupción por flanco de subida en un pin GPIO.
• Pines: Utilice un pin del Puerto 0 como entrada para el pulsador y 7 pines del Puerto 2 para el display de 7 segmentos.
• Condiciones:
	* Configure el pin de entrada para interrumpir solo cuando se detecte un flanco de subida.
	* La lógica de incremento del contador debe residir en el Handler de la Interrupción (ISR).

 * */

#include "LPC17xx.h"
#include <stdbool.h>
#include <stdio.h>

const uint8_t symbols [16]= {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x67, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71  // F
};


void configPinsDisplay (){

	LPC_PINCON->PINSEL4 &= ~ (0XFFFF << 0);
	LPC_PINCON ->PINMODE4 |= (0XFFFF << 0);

	LPC_GPIO2 ->FIODIR |= (0XFF << 0);
	LPC_GPIO2 ->FIOCLR |= (0XFF << 0);
}

void configEint3 (){
	LPC_PINCON-> PINSEL0 &= ~ (0B10 << 0);
	LPC_PINCON->PINMODE0 |= (3 << 0);

	LPC_GPIO0->FIODIR &= ~ (1 << 0);


	LPC_GPIOINT ->IO0IntEnR |= (1<<0);

	NVIC_EnableIRQ(EINT3_IRQn);
}
void showDisplay(uint8_t c){
	LPC_GPIO0->FIOCLR |= (0X7F <<0);
	LPC_GPIO0->FIOSET = (symbols[c]  & 0X7F);
}
void EINT3_IRQHandler(){
	static uint8_t counter = 0;

	if (LPC_GPIOINT ->IO0IntStatR & (1 << 0)){
		counter = (counter +1 )%16;
		showDisplay(counter);
		LPC_GPIOINT->IO0IntClr |= (1 << 0 );

	}
}

int main (){
	configPinsDisplay();
	configEint3();
	while(1){
		__WFI();
	}
}
