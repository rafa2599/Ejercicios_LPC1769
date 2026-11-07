/* Desarrolle un programa que haga parpadear un color en el LED RGB integrado en la placa.
• Pines: Rojo (P0.22), Verde (P3.25), Azul (P3.26). Los LEDs son activos por bajo (se encienden con un 0
lógico).
• Condiciones:
o Modularice el código utilizando funciones como configGPIO() y delay().
o Implemente un retardo por software (con un bucle for).
o Defina el tiempo del retardo con una macro (#define).
 * */

#include "LPC17xx.h"
#include <LPC17xx.h>
#define PORT0_22 (1<<22)
#define PORT3_25 (1<<25)
#define PORT3_26 (1<<26)

#define DELAY 3000000000U

void configPines (){

	LPC_PINCON->PINSEL0  &= ~(3<<12); // P0_22 MODO GPIO
	LPC_PINCON->PINSEL7 &= ~(3<<18); // P3_25 MODO GPIO
	LPC_PINCON->PINSEL7 &= ~(3<<20); // P3_26 MODO GPIO

	LPC_PINCON ->PINMODE1 &= ~(3 << 12);
	LPC_PINCON ->PINMODE7 &= ~(3<<18);
	LPC_PINCON ->PINMODE7 &= ~(3<<20);

	LPC_GPIO0 -> FIODIR |= PORT0_22;
	LPC_GPIO3 -> FIODIR |= PORT3_25;
	LPC_GPIO3 -> FIODIR |= PORT3_26;
}

void delay(uint23_t delayparam){
	for (volatile uint32_t i = 0 ; i < delayparam ; i++){
		__NOP();
	}

}
int main (){
	configPines();
	while(1){
		LPC_GPIO0 ->FIOSET |=PORT0_22;
		delay(DELAY);
		LPC_GPIO0 ->FIOCLR |= PORT0_22;
		delay(DELAY);
		LPC_GPIO3 ->FIOSET |=PORT3_25;
		delay(DELAY);
		LPC_GPIO3 ->FIOCLR |=PORT3_25;
		delay(DELAY);
		LPC_GPIO3 ->FIOSET |=PORT3_26;
		delay(DELAY);
		LPC_GPIO3 ->FIOCLR |=PORT3_26;
		delay(DELAY);
	}
	return 0;
};
