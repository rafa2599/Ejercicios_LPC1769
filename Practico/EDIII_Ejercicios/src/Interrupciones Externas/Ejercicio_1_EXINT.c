/* Ejercicio 1: Toggle de LED mediante Interrupción Dedicada (EINT0)
	* Desarrolle un programa muy sencillo que utilice un pin del Puerto 2 como interrupción externa (EINT0). Cada
	* vez que se active la interrupción por flanco de bajada, un LED conectado a otro pin debe cambiar de estado
	* (encenderse si estaba apagado, y apagarse si estaba encendido).
	• Pines: Utilice P2.10 para el botón (EINT0) y P0.22 para el LED.
 */
#include "LPC17xx.h"
#include <stdbool.h>
#include <stdio.h>

volatile static bool led_state = false;

void confPinLed (){
	LPC_PINCON->PINSEL1 &=~ (3 << 12);
	LPC_PINCON->PINMODE1 |= (3 << 12);

	LPC_GPIO0->FIODIR |= (1 <<22);
	LPC_GPIO0->FIOCLR |= (1 <<22);
}
void configEint0(void) {
    // Configura P2.10 con funcion EINT0 (bits 21:20 = 01)
    LPC_PINCON->PINSEL4 &= ~(3 << 20);
    LPC_PINCON->PINSEL4 |= (1 << 20);

    // Configura EINT0 por flanco (EXTMODE bit 0 = 1)
    LPC_SC->EXTMODE |= (1 << 0);

    // Configura EINT0 por flanco de BAJADA (EXTPOLAR bit 0 = 0)
    LPC_SC->EXTPOLAR &= ~(1 << 0);

    // Limpia bandera pendiente previa de EINT0
    LPC_SC->EXTINT |= (1 << 0);

    // Habilita interrupcion EINT0 en el NVIC
    NVIC_EnableIRQ(EINT0_IRQn);
}
void EINT0_IRQHandler(void) {
    // Alterna el estado del LED


    if (led_state) {
        LPC_GPIO0->FIOSET = (1 << 22); // Enciende LED
    } else {
        LPC_GPIO0->FIOCLR = (1 << 22); // Apaga LED
    }
    led_state = !led_state;
    // Limpia la bandera de interrupcion EINT0
    LPC_SC->EXTINT |= (1 << 0);
}

int main(void) {
    confPinLed();
    configEint0();

    while (1) {
        __WFI(); // Modulo de bajo consumo a la espera de interrupción
    }
    return 0;
}
