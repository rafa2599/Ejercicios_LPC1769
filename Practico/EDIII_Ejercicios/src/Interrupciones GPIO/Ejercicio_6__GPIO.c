#include "LPC17xx.h"
#include <cr_section_macros.h>
#include <stdio.h>
/*Conecta 6 LEDs a los pines P2.0 al P2.5. Diseña una función que desplace una luz encendida de izquierda a derecha (P2.0 -> P2.5)
 * y luego de regreso (P2.5 -> P2.0) en un ciclo infinito.
 *
 * */
#define delayTime 2500000
void confPines(){
	LPC_PINCON->PINSEL4 &= ~ (0x0FFF);
	LPC_GPIO2 -> FIODIR |= (0X3F);
	LPC_GPIO2 -> FIOMASK &= ~ (0X3F);
	LPC_GPIO2 -> FIOCLR |= (0X3F);
}
void delay(uint32_t d){

	for (volatile uint32_t i = 0 ; i < d ; i ++){

		__NOP();
	}
}
void blinkLed(uint32_t idx, uint32_t d){

	LPC_GPIO2->FIOSET |= (1 << idx);
	delay(d);
	LPC_GPIO2->FIOCLR |= (1 << idx);
	delay(d);
}

int main(){

	confPines();

	while (1){

		for (volatile uint32_t i = 0 ; i < 6 ; i ++){

			blinkLed(i,delayTime);
		}
		for (volatile uint32_t i = 5 ; i > 0; i--){

			blinkLed(i, delayTime);
		}
	}

};
