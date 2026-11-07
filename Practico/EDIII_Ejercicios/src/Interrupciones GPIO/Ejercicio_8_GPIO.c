/*
Configura el pin P2.10 como entrada digital con resistencia de Pull-Up interna habilitada.
Habilita la interrupción en P2.10 para responder a ambos flancos (ascendente y descendente):
Cuando el usuario presione el botón (flanco descendente, (3.3 V ->0 V), el LED en P0.22 debe encenderse.
Cuando el usuario suelte el botón (flanco ascendente, (0 V ->3.3 V), el LED en P0.22 debe apagarse.
  */

#include "LPC17xx.h"
#include <cr_section_macros.h>
#include <stdio.h>

#define PIN_LED (1<<22)
#define PIN_BTN (1<<10)

void confPinLed(){
	LPC_PINCON ->PINSEL1 &= ~ (3 << 12);
	LPC_GPIO0 ->FIODIR |= (PIN_LED);
	LPC_GPIO0 ->FIOMASK &= ~ (PIN_LED);
	LPC_GPIO0 ->FIOCLR |= (PIN_LED);

}

void confPinBoton (){

	LPC_PINCON ->PINSEL4 &=~ (3 << 20);
	LPC_GPIO2 ->FIODIR &= ~ (PIN_BTN);
	LPC_PINCON->PINMODE4 &= ~ (3<<20);

	// habilitar interrupción en P2.10 por AMBOS flancos
	LPC_GPIOINT->IO2IntEnF |= PIN_BTN; // Flanco descendente (al presionar)
	LPC_GPIOINT->IO2IntEnR |= PIN_BTN; // Flanco ascendente (al soltar)
	// 5\. Limpiar cualquier bandera de interrupción previa en Puerto 2
	LPC_GPIOINT->IO2IntClr = PIN_BTN;
	// 6\. Habilitar la línea de interrupción compartidaEINT3/GPIO en el NVIC
	NVIC_EnableIRQ(EINT3_IRQn);
}



void EINT3_IRQHandler(void)
{
	if (LPC_GPIOINT -> IO2IntStatF & (PIN_BTN)){
		LPC_GPIO0->FIOSET |= (PIN_LED);
		LPC_GPIOINT ->IO2IntClr |= (PIN_BTN);
	}else if (LPC_GPIOINT ->IO2IntStatR & (PIN_BTN)){
		LPC_GPIO0 ->FIOCLR |= (PIN_LED);
		LPC_GPIOINT ->IO2IntClr |= (PIN_BTN);
	}
}

int main (void){

	confPinLed();
	confPinBoton();
	while(1){
		__WFI();
	}
	return 0;
};
