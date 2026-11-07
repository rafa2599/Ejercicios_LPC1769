/*Desarrolle un programa que permita leer el estado de todos los pines disponibles del Puerto 0 y cuente
cuántos de ellos están en un nivel alto (1 lógico). El resultado debe mostrarse en binario utilizando 5 LEDs
conectados a los pines menos significativos del Puerto 2.
 */
#include "LPC17xx.h"

#define DELAY 3000000000U
void configPinesPort0 (){

	LPC_PINCON ->PINSEL0 &= ~(0xFFFFFFFF);
	LPC_PINCON ->PINSEL1 &= ~(0xFFFFFFFF);
	LPC_PINCON ->PINMODE0 &= ~(0xFFFFFFFF);

	LPC_GPIO0 ->FIODIR &= ~ (0xFFFFFFFF);
	LPC_GPIO0 ->FIOCLR |= (0xFFFFFFFF);

}
void configPinesPort2 (){
	LPC_PINCON ->PINSEL4 &= ~(0XFFF<<0);
	LPC_PINCON ->PINMODE4&= ~(0XFFF<<0);

	LPC_GPIO2 ->FIODIR |= (0X1F <<0);
	LPC_GPIO2 ->FIOCLR |= (0X1F <<0);
}
void delay (uint32_t d){
	for (volatile uint32_t i = 0; i<d ; i++){
		__NOP();
	}
}
uint8_t countPins (){

	uint32_t pinsUp = (LPC_GPIO0->FIOPIN & 0XFFFFFFFF);
	uint8_t contador= 0;
	for (volatile uint8_t i = 0 ; i<32 ;i++){

		if (pinsUp &(1U << i)){
			contador ++;
		}
	}
	return contador ;
}
void display2(uint8_t valor){
	LPC_GPIO2 ->FIOCLR =(0X1F);
	delay(DELAY);
	LPC_GPIO2->FIOSET = (0X1F & valor);
}

int main (){
	configPinesPort0();
	configPinesPort2();
	while(1){
		uint8_t v = countPins();
		display2(v);
	}
};
