#include "LPC17xx.h"
#include <cr_section_macros.h>
#include <stdio.h>

// Definición de constantes para legibilidad
#define LED_PIN (1 << 22)

// Prototipos de funciones
void delay(void);
void confPorts(void);
void blinkLeds(void);

int main(void) {
    confPorts();   // Inicializa los periféricos y puertos
    blinkLeds();   // Ejecuta el lazo de parpadeo infinito
    return 0;
}

void confPorts(void) {
    // 1. Configurar P0.22 como GPIO (limpia bits 13:12 en PINSEL1)
    LPC_PINCON->PINSEL1 &= ~(0b11 << 12);

    // 2. Configurar P0.22 como salida digital
    LPC_GPIO0->FIODIR |= LED_PIN;

    // 3. Habilitar el acceso al pin desenmascarándolo
    LPC_GPIO0->FIOMASK &= ~LED_PIN;

    // 4. Estado inicial: LED apagado
    LPC_GPIO0->FIOCLR |= LED_PIN;
}

void delay(void) {
    static uint32_t delayParam = 3000000;

    for (volatile uint32_t i = 0; i < delayParam; i++) {
        __NOP(); // Instrucción NOP (1 ciclo de CPU)
    }
}

void blinkLeds(void) {
    while (1) {
        LPC_GPIO0->FIOSET |= LED_PIN; // Encender LED
        delay();
        LPC_GPIO0->FIOCLR |= LED_PIN; // Apagar LED
        delay();
    }
}

