/*Desarrolle un programa que lea un valor de 4 bits en formato binario (0-F) de 4 pines de entrada. Este valor
debe ser decodificado y mostrado en un display de 7 segmentos.
 * */
#include "LPC17xx.h"
const uint8_t symbols [16]={
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
void configPinInput(){
	//Configuramos los pines : P0.0 al P0.3
	LPC_PINCON ->PINSEL0 &= ~ (0XFF << 0);
	LPC_PINCON ->PINMODE0 |=  (0XFF << 0);

	//Los configuramos como entradas
	LPC_GPIO0 ->FIODIR &= ~ (0XF << 0);

}
void configPinOutput (){
	LPC_PINCON ->PINSEL4 &= ~ (0x3FFF<<0);
	LPC_PINCON ->PINMODE4 |= (0x3FFF<<0);

	LPC_GPIO2->FIODIR |= (0X7F<<0);
	LPC_GPIO2->FIOCLR |= (0X7F<<0);

}

uint8_t checkPins(void) {
    // Lee el valor binario de los 4 pines de entrada (P0.0 a P0.3)
    return (LPC_GPIO0->FIOPIN & 0x0F);
}


void showNumber(uint8_t input){

	for (volatile uint8_t i = 0 ; i<16 ; i++){

		if (symbols[i]==input){
			LPC_GPIO2->FIOSET = (0x7F) & (symbols[i]);
		}
		else {
			LPC_GPIO2->FIOSET = (0x7F) & (symbols[0]);
		}
	}
}

int main (){
	 configPinInput();
	 configPinOutput ();

	 while(1){
		 uint8_t n = checkPins();
		 showNumber(n);
		 LPC_GPIO2 ->FIOCLR |= (0X7F);
	 }
}
