#include "LPC17xx.h"
#include <cr_section_macros.h>
#include <stdio.h>

#define delayTime 5000000

const  uint32_t  counterLed [16] =
		{0b0000,
		0b0001,
		0b0010,
		0b0011,
		0b0100,
		0b0101,
		0b0110,
		0b0111,
		0b1000,
		0b1001,
		0b1010,
		0b1011,
		0b1100,
		0b1101,
		0b1110,
		0b1111
		};

void confPorts(){
	LPC_PINCON ->PINSEL0 &= ~(0xFF << 8);

	LPC_GPIO0 ->FIODIR |= (0X0F<< 4);
	LPC_GPIO0 ->FIOMASK &= ~ (0X0F << 4);
	LPC_GPIO0 ->FIOCLR |= (0X0F << 4);

}
void delay(uint32_t delayParam){

	for (volatile uint32_t i = 0 ; i < delayParam ; i++){
		__NOP();
	}
}
void blinkLeds (uint32_t x , uint32_t d){
	LPC_GPIO0 ->FIOSET |= (x << 4);
	delay (d);
	LPC_GPIO0 ->FIOCLR |= (x << 4);
	delay(d);

}
int main (void){
	confPorts();
	while (1){

		for (volatile uint32_t i = 0 ; i < 16 ; i++){

			blinkLeds (counterLed[i],delayTime);
		}
	}

};
