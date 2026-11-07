/*Desarrolle un programa que haga parpadear un LED cada 500 ms utilizando el SysTick Timer.
• Pines: Utilice el pin P0.22 para el LED (activo por bajo).
• Condiciones:
     * Configure el SysTick para generar una interrupción cada 500 ms.
 	 * El programa principal (main) debe estar en un bucle infinito vacío, demostrando que la tarea de parpadeo se gestiona completamente por la interrupción.
 * */

#include "LPC17xx.h"
#include <stdbool.h>

#define nTicks 9999999
#define control_bits (1<<0)|(1<<1)|(1<<2)
void configLed(){
	LPC_PINCON->PINSEL0 &= ~ (3 << 12);
	LPC_PINCON->PINMODE0 &= ~  (3 << 12);

	LPC_GPIO0->FIODIR |= (1 << 22);
	LPC_GPIO0->FIOCLR |= (1 << 22);
}
void configSystick (){
	SysTick->CTRL = control_bits;
	SysTick ->LOAD = nTicks;
	SysTick ->VAL = 0;
	NVIC_SetPriority(SysTick_IRQn, 0);
}
void SysTick_IRQHandler(){
	static uint8_t counter = 0;
	if (counter == 5){
		LPC_GPIO0 ->FIOSET ^=  (1 << 22);
		counter = 0;

	}else {counter ++;}
}

int main (){
	configLed();
	configSystick ();
	while(1){
		__WFI();
	}
	return 0;
}
