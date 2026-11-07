/*Escriba una rutina simple para configurar e iniciar un Systick que pueda interrumpir cada 10 ms. Escriba
también una rutina mínima del handler.
 * */
#include "LPC17xx.h"
#include <stdbool.h>

#define nTicks 999999
#define control_bits (1<<0)|(1<<1)|(1<<2)
void configLed(){
	LPC_PINCON->PINSEL0 &= ~ (3 << 12);
	LPC_PINCON->PINMODE0 |= (3 << 12);

	LPC_GPIO0->FIODIR |= (1 << 22);
	LPC_GPIO0->FIOCLR |= (1 << 22);
}
void configSystick (){
	SysTick->CTRL = control_bits;
	SysTick ->LOAD = nTicks;
	SysTick ->VAL = 0;
	NVIC_SetPriority(SysTick_IRQn, 0);


}
void SysTick_Handler(void) {
    // Código a ejecutar periódicamente en cada tick de tiempo
    LPC_GPIO0->FIOPIN ^= (1 << 22); // Toggle de un LED en P0.22
}
int main(){
	configLed();
	configSystick();
	while(1){
		__WFI();
	}
	return 0;
}
