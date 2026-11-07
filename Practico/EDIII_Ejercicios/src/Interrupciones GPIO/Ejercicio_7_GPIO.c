/* ===============================================================================
* @file Ejercicio\_Nivel1.c
* @brief Conmuta (Toggle) el estado lógico de un LED conectado al pin P0.22
* cada vez que se detecta un flanco descendente en el pin P0.15
* utilizando interrupciones por GPIO.
*==============================================================================*/
#include "LPC17xx.h"
#include <cr_section_macros.h>
#include <stdio.h>

#define PIN_LED (1 << 22) // P0.22
#define PIN_BTN (1 << 15) // P2.10

void confPines(){

	//Configuro P0.22 como gpio
	LPC_PINCON ->PINSEL1 &= ~(3<<12);
	//seteo la salida como digital
	LPC_GPIO0 ->FIODIR |= PIN_LED;
	//Seteo el fiomask = 0
	LPC_GPIO0 ->FIOMASK &= ~ PIN_LED;
	//pongo la salida en 0
	LPC_GPIO0 ->FIOCLR |= PIN_LED;

	//Configuro P2.10 como gpio
	LPC_PINCON ->PINSEL4 &= ~(3<<30);
	//seteo la entrada como digital
	LPC_GPIO0 ->FIODIR &= ~(PIN_BTN);
	//Habilito la interrupcion por fd
	LPC_GPIOINT ->IO0IntEnF |= (PIN_BTN);
	//Limpio la flag
	LPC_GPIOINT ->IO0IntClr |= (PIN_BTN);

	NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void)
{
    static uint32_t index = 0; // Variable con almacenamiento persistente

    if (LPC_GPIOINT->IntStatus & (1 << 0))
    {
        if (LPC_GPIOINT->IO0IntStatF & (1 << 15))
        {
        	LPC_GPIO0->FIOPIN ^= (1<<22);

            LPC_GPIOINT->IO0IntClr |= (1 << 15); // Limpieza de bandera
        }
    }
}


int main (void){

	confPines();
	while (1){
		__WFI();
	}
};
