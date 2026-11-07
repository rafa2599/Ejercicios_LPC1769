/*===============================================================================
* @file     Ejercicio_2_2.c
* @brief    Controla un LED (P0.22) alternando su tiempo de parpadeo según
*           el estado lógico de una entrada digital (P2.10).
*==============================================================================*/
#include "LPC17xx.h"
#include <cr_section_macros.h>
#include <stdio.h>

#define delayTimeMAX  5000000
#define delayTimeMIN  2500000

void confPins(void);
void delay(uint32_t);
void blinkLed(uint32_t);

int main(void) {
    // 1. Inicialización obligatoria de pines
    confPins();

    // 2. Lazo de control principal
    while (1) {
        // Leemos el pin P2.10 usando FIOPIN
        if (LPC_GPIO2->FIOPIN & (1 << 10)) {
            blinkLed(delayTimeMAX); // Botón abierto ( Pull-Up = 1 lógico )
        } else {
            blinkLed(delayTimeMIN); // Botón presionado ( Conectado a GND = 0 )
        }
    }

    return 0;
}

void confPins(void) {
    // Configuración P0.22 (LED)
    LPC_PINCON->PINSEL1 &= ~(0b11 << 12); // Función GPIO
    LPC_GPIO0->FIOMASK  &= ~(1 << 22);    // Desenmascarar
    LPC_GPIO0->FIODIR   |= (1 << 22);     // Salida
    LPC_GPIO0->FIOCLR   |= (1 << 22);     // LED Apagado por defecto

    // Configuración P2.10 (Pulsador)
    LPC_PINCON->PINSEL4  &= ~(0b11 << 20); // Función GPIO
    LPC_PINCON->PINMODE4 &= ~(0b11 << 20); // Pull-Up interno habilitado
    LPC_GPIO2->FIOMASK  &= ~(1 << 10);     // Desenmascarar
    LPC_GPIO2->FIODIR   &= ~(1 << 10);     // Entrada
}

void delay(uint32_t timeDelay) {
    for (volatile uint32_t i = 0; i < timeDelay; i++) {
        __NOP();
    }
}

void blinkLed(uint32_t timeDelay) {
    LPC_GPIO0->FIOSET |= (1 << 22); // Encender LED
    delay(timeDelay);
    LPC_GPIO0->FIOCLR |= (1 << 22); // Apagar LED
    delay(timeDelay);
}
