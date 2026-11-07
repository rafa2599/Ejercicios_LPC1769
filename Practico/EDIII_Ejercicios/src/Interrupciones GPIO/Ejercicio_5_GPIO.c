/*
 * Se cuenta con un LED en P0.22 y dos pulsadores externos1:Botón 1 (P2.10): Conectado con lógica de Pull-Up interna.
 * Botón 2 (P2.11): Conectado con lógica de Pull-Down interna4.
 * Comportamiento del sistema:
 * Si ningún botón se presiona -> El LED permanece apagado.
 * Si se presiona solo Botón 1 -> El LED parpadea rápido.
 * Si se presiona solo Botón 2 ->El LED parpadea lento.
 * Si se presionan ambos botones simultáneamente -> El LED se mantiene encendido fijo
 * */
#include "LPC17xx.h"
#include <cr_section_macros.h>
#include <stdio.h>

#define slowBlink 5000000
#define fastBlink 2500000


void confPines(){

	//PRIMERO PONER EL PINSEL EN 00
	LPC_PINCON ->PINSEL1 &= ~ (0b11 << 12);
	//SEGUNDO PONER EL PIN COMO SALIDA DIGITAL FIODIR EN 1
	LPC_GPIO0 ->FIODIR |= (1 << 22);
	//TERCERO CONFIGURAR SU MASCARA FIOMASK EN 0
	LPC_GPIO0->FIOMASK &= ~(1 << 22);
	//CUARTO  PONER SU ESTADO EN BAJO FIOCLR EN 1
	LPC_GPIO0 ->FIOCLR |= (1<< 22);

	//------------------------------------------//
	//Primero ponemos los pines 10 y 11 en modo GPIO
	LPC_PINCON ->PINSEL4 &= ~ (0b1111 << 20);
	// Segundo desactivamos el FIOMASK = 0
	LPC_GPIO2 ->FIOMASK &= ~ (0b11 << 10);
	//Tercero ponemos los pines como entradas
	LPC_GPIO2 -> FIODIR &= ~ (0b11 << 10);
	//Cuarto seteamos las resistencias pull-up/down
	LPC_PINCON->PINMODE4 |= (0b11 << 22);
	LPC_PINCON->PINMODE4 &= ~ (0b11 << 20);

}

void delay (uint32_t d){

	for (volatile uint32_t i = 0 ; i< d ; i++){

		__NOP();
	}
}
void blinkLed(uint32_t delayTime){
	LPC_GPIO0 ->FIOSET |= (1 << 22);
	delay(delayTime);
	LPC_GPIO0 ->FIOCLR |= (1 << 22);
	delay(delayTime);
}
int main (void){
	confPines();

	uint32_t condicionB1 = LPC_GPIO2 -> FIOPIN & (0b01 << 10);
	uint32_t condicionB2 = LPC_GPIO2 -> FIOPIN & (0b01 << 11);

	while (1){
		if (condicionB1 == 0 && condicionB2 == 0){
			while (condicionB1 != 0 || condicionB2 != 0){

				LPC_GPIO0 ->FIOCLR |= (1 <<22);
			}
		}else if (condicionB1 == 0 && condicionB2 !=0){
			blinkLed (slowBlink);

		}else if (condicionB1 == 1 && condicionB2 !=1){
			blinkLed(fastBlink);
		}else {LPC_GPIO0 ->FIOSET |= (1 <<22);}

	}
};
