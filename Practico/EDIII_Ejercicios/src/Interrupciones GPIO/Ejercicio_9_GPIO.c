/*Diseña un sistema con dos botones en el **Puerto 2**:

* Botón A (P2.0)Configurado con interrupción por (flanco ascendente).
Al presionarse, debe cambiar el tiempo de retardo de un LED en `P0.22` a modo RÁPIDO.

* Botón B (P2.1) Configurado con interrupción por (flanco descendente).
* Al presionarse, debe cambiar el tiempo de retardo del LED a modo ENTO.

En el bucle `while(1)`, el programa principal se limita a hacer parpadear el LED utilizando la variable global de tiempo actualizada por la ISR
 * */

#include "LPC17xx.h"
#include <cr_section_macros.h>
#include <stdio.h>

#define PIN_LED  (1 << 22) // P0.22[cite: 3, 5]
#define PIN_BTN1 (1 << 0)  // P2.0
#define PIN_BTN2 (1 << 1)  // P2.1[cite: 1]
#define PIN_LED_MODE (3 << 12)
#define PIN_BTN1_MODE (0b11<<0)
#define PIN_BTN2_MODE (0b11<<2)

#define CLEAR_MASK(REG, MASK)   ((REG) &= ~(MASK))
#define SET_MASK(REG, MASK)     ((REG) |= (MASK))
#define TOGGLE_MASK(REG, MASK)  ((REG) ^= (MASK))

#define BlinkSlow 4000000
#define BlinkFast 1000000

void confPinLed(){
	CLEAR_MASK (LPC_PINCON->PINSEL1,PIN_LED_MODE); //Ponemos el pin P0.22 en modo GPIO (EN 00)
	SET_MASK (LPC_GPIO0->FIODIR,PIN_LED);      // Ponemos el pin P0.22 como salida digital FIODIR =1
	CLEAR_MASK (LPC_GPIO0 ->FIOMASK,PIN_LED); // Deshabilitamos la mascara FIOMASK = 0
	SET_MASK (LPC_GPIO0->FIOCLR,PIN_LED);    //  Seteamos la salida en cero FIOCLR = 1

}

void confPinButton(){

	//config. Boton 1
	CLEAR_MASK (LPC_PINCON->PINSEL4,PIN_BTN1_MODE); //Ponemos el pin P2.0 en modo GPIO (EN 00)
	CLEAR_MASK (LPC_GPIO2 ->FIODIR,PIN_BTN1);      //Ponemos el pin p2.0 como entrada digital FIODIR = 0
	CLEAR_MASK (LPC_GPIO2 ->FIOMASK,PIN_BTN1); // Deshabilitamos la mascara FIOMASK = 0
	SET_MASK (LPC_PINCON ->PINMODE4,PIN_BTN1_MODE); // Hablilitamos la resistencia PULL DOWN = 11

	// confg. Boton 2
	CLEAR_MASK (LPC_PINCON ->PINSEL4,PIN_BTN2_MODE);
	CLEAR_MASK (LPC_GPIO2 ->FIODIR,PIN_BTN2);      //Ponemos el pin p2.1 como entrada digital FIODIR = 0
	CLEAR_MASK (LPC_GPIO2 ->FIOMASK,PIN_BTN2); // Deshabilitamos la mascara FIOMASK = 0
	CLEAR_MASK (LPC_PINCON ->PINMODE4,PIN_BTN2_MODE); // Hablilitamos la resistencia PULL UP = 00
	// Habilitamos las interrupciones por flacon
	SET_MASK (LPC_GPIOINT ->IO2IntEnR,PIN_BTN1); // para P2.0 por flanco ascendente
	SET_MASK (LPC_GPIOINT ->IO2IntEnF,PIN_BTN2); // para P2.1 por flanco descendente
	//Limpiamos las flags
	SET_MASK (LPC_GPIOINT->IO2IntClr,PIN_BTN1);
	SET_MASK (LPC_GPIOINT->IO2IntClr,PIN_BTN2);

	NVIC_EnableIRQ(EINT3_IRQn);
}
void delay(uint32_t delayParam){
	for (volatile uint32_t i = 0 ; i < delayParam ; i ++){
		__NOP();
	}
}
void blinkLed (uint32_t d){
	SET_MASK (LPC_GPIO0->FIOSET,PIN_LED);
	delay(d);
	SET_MASK (LPC_GPIO0->FIOCLR,PIN_LED);
	delay(d);
}

void EINT3_IRQHandler(void){

	if (LPC_GPIOINT ->IO2IntStatR & (PIN_BTN1)){

		blinkLed(BlinkFast);
		SET_MASK (LPC_GPIOINT->IO2IntClr,PIN_BTN1);
	}else if (LPC_GPIOINT ->IO2IntStatF & (PIN_BTN2)){
		blinkLed(BlinkSlow);
		SET_MASK (LPC_GPIOINT->IO2IntClr,PIN_BTN2);
	}
}

int main (void){
	confPinLed();
	confPinButton();
	while(1){__WFI();}
	return 0;
};
