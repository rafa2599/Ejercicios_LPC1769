/*
 *Desarrolle un programa que utilice 7 pines GPIO para controlar un display de 7 segmentos. Se debe mostrar
de manera cíclica y automática los 16 dígitos hexadecimales (0-F) de forma ascendente.
  */
/* |g|f|e|d|c|b|a|
 0b 0 1 1 1 1 1 1    -> 0
 0b 0 0 0 0 1 1 0    -> 1
 0b 1 0 1 1 0 1 1    -> 2
 0b 1 0 0 1 1 1 1    -> 3
 0b 1 1 0 0 1 1 0    -> 4
 0b 1 1 0 1 1 0 1    -> 5
 0b 1 1 1 1 1 0 1    -> 6
 0b 0 0 0 0 1 1 1    -> 7
 0b 1 1 1 1 1 1 1    -> 8
 0b 1 1 0 0 1 1 1    -> 9
  * */

#include "LPC17xx.h"
#define DELAY 10000000U

const uint8_t symbols[16] = {
    0b00111111, // 0 (0x3F)
    0b00000110, // 1 (0x06)
    0b01011011, // 2 (0x5B)
    0b01001111, // 3 (0x4F)
    0b01100110, // 4 (0x66)
    0b01101101, // 5 (0x6D)
    0b01111101, // 6 (0x7D)
    0b00000111, // 7 (0x07)
    0b01111111, // 8 (0x7F)
    0b01100111, // 9 (0x67)
    0b01110111, // A (0x77)
    0b01111100, // b (0x7C)
    0b00111001, // C (0x39)
    0b01011110, // d (0x5E)
    0b01111001, // E (0x79)
    0b01110001  // F (0x71)
};

void configPines (){
	LPC_PINCON ->PINSEL4 &=~ (0X3FFF<<4);
	LPC_PINCON ->PINMODE4 |= (0X3FFF<<4);

	LPC_GPIO2 ->FIODIR |= (0X7F << 2);
	LPC_GPIO2 ->FIOCLR |= (0X7F << 2);
}
void delay (uint32_t d){
	for (volatile uint32_t i = 0; i<d ; i++){
		__NOP();
	}
}
void blink (uint32_t d){
	for (volatile uint32_t i = 0 ; i < 16 ; i ++){
			LPC_GPIO2 ->FIOSET = (simbols [i] << 2);
			delay (d);
			LPC_GPIO2 ->FIOCLR = (0X7F << 2);
	}
}


int main(){

	configPines();
	while(1){
		blink(DELAY);
	}
	return 0;
}
