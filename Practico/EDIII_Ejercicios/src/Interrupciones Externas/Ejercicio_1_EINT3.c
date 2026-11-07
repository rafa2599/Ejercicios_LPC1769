
/* Ejercicio 1: Toggle de LED mediante Interrupción Dedicada (EINT0)
	* Desarrolle un programa muy sencillo que utilice un pin del Puerto 2 como interrupción externa (EINT0). Cada
	* vez que se active la interrupción por flanco de bajada, un LED conectado a otro pin debe cambiar de estado
	* (encenderse si estaba apagado, y apagarse si estaba encendido).
	• Pines: Utilice P2.10 para el botón (EINT0) y P0.22 para el LED.
 */
#include "LPC17xx.h"
#include <stdbool.h>
#include <stdio.h>

volatile static bool on = false;

void confPinLed (){
	LPC_PINCON->PINSEL1 &=~ (3 << 12);
	LPC_PINCON->PINMODE1 |= (3 << 12);

	LPC_GPIO0->FIODIR |= (1 <<22);
	LPC_GPIO0->FIOCLR |= (1 <<22);
}
void configEint3(){
	LPC_PINCON->PINSEL4 &= ~ (0B10 << 20);
	LPC_PINCON->PINMODE4 &= ~ (3 << 20);

	LPC_GPIO2 -> FIODIR &= ~ (1 << 10);

	LPC_GPIOINT->IO2IntEnF |= (1 <<10);
	NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQnHandler(){
	if (LPC_GPIOINT->IO2IntStatF & (1 <<10)){
		if (on == false){
			LPC_GPIO0->FIOSET |= (1 << 22);

		}else {LPC_GPIO0->FIOCLR |=  (1 << 22);}
	}
	on = !on;
	LPC_GPIOINT->IO2IntClr |= (1<<10);
}

int main (void){
	confPinLed();
	configEint3();
	while(1){__WFI();}
	return 0;
};
