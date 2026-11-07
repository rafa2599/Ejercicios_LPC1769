/*Desarrolle un programa que use dos fuentes de interrupción para controlar secuencias de LEDs diferentes.
• Configuración:
	- Configure el pin P0.0 para generar una interrupción por flanco de subida.
	* Configure una interrupción externa (EINT1) en el pin P2.11 por flanco de bajada.
• Lógica:
	* Cuando se detecta la interrupción del pin P0.0, se debe ejecutar una secuencia de 4 LEDs o 4 números si está utilizando un display de 7 segmentos
	* Cuando se detecta la interrupción externa del pin P2.11, se debe ejecutar una secuencia de parpadeo diferente.
• Condiciones: Configure la interrupción externa (EINT1) para que tenga una mayor prioridad que la
interrupción del GPIO del Puerto 0.
 * */
#include "LPC17xx.h"
#include <stdbool.h>

void configLed0(void) {
    // Configura P0.2 a P0.5 como GPIO (bits 11:4 de PINSEL0 en 0)
    LPC_PINCON->PINSEL0 &= ~(0xFF << 4);

    // P0.2 a P0.5 como salidas digitales
    LPC_GPIO0->FIODIR |= (0x0F << 2);
    LPC_GPIO0->FIOCLR = (0x0F << 2);
}

void configLed1(void) {
    // Configura P2.0 a P2.3 como GPIO (bits 7:0 de PINSEL4 en 0)
    LPC_PINCON->PINSEL4 &= ~(0xFF << 0);

    // P2.0 a P2.3 como salidas digitales
    LPC_GPIO2->FIODIR |= (0x0F << 0);
    LPC_GPIO2->FIOCLR = (0x0F << 0);
}

void configIntGPIO_P0(void) {
    // P0.0 como GPIO (bits 1:0 de PINSEL0 en 0)
    LPC_PINCON->PINSEL0 &= ~(3 << 0);

    // P0.0 como entrada
    LPC_GPIO0->FIODIR &= ~(1 << 0);

    // Interrupción por flanco de subida en P0.0
    LPC_GPIOINT->IO0IntEnR |= (1 << 0);
    LPC_GPIOINT->IO0IntClr = (1 << 0); // Limpia flag pendiente

    // Configura prioridad en el NVIC (Prioridad menor = 1)
    NVIC_SetPriority(EINT3_IRQn, 1);
    NVIC_EnableIRQ(EINT3_IRQn);
}

void configEint1(void) {
    // Configura P2.11 con función EINT1 (bits 23:22 de PINSEL4 en 01)
    LPC_PINCON->PINSEL4 &= ~(3 << 22);
    LPC_PINCON->PINSEL4 |= (1 << 22);

    // EINT1 por flanco descendente
    LPC_SC->EXTMODE |= (1 << 1);    // 1 = Por Flanco
    LPC_SC->EXTPOLAR &= ~(1 << 1);  // 0 = Descendente
    LPC_SC->EXTINT |= (1 << 1);     // Limpia flag pendiente EINT1

    // Configura mayor prioridad en el NVIC (Prioridad mayor = 0)
    NVIC_SetPriority(EINT1_IRQn, 0);
    NVIC_EnableIRQ(EINT1_IRQn);
}

void delay(void) {
    for (volatile uint32_t i = 0; i < 1000000; i++) {
        __NOP();
    }
}

// ISR para interrupciones por GPIO (P0.0)
void EINT3_IRQHandler(void) {
    if (LPC_GPIOINT->IO0IntStatR & (1 << 0)) {
        LPC_GPIO2->FIOCLR = (0x0F << 0); // Apaga LEDs del otro grupo

        // Secuencia en LEDs P0.2 a P0.5
        for (uint8_t i = 0; i < 4; i++) {
            LPC_GPIO0->FIOSET = (0x0F << 2);
            delay();
            LPC_GPIO0->FIOCLR = (0x0F << 2);
            delay();
        }

        // Limpia la bandera de interrupción por GPIO
        LPC_GPIOINT->IO0IntClr = (1 << 0);
    }
}

// ISR para Interrupción Externa EINT1 (P2.11)
void EINT1_IRQHandler(void) {
    LPC_GPIO0->FIOCLR = (0x0F << 2); // Apaga LEDs del otro grupo

    // Secuencia diferente en LEDs P2.0 a P2.3
    for (uint8_t i = 0; i < 4; i++) {
        LPC_GPIO2->FIOSET = (0x0F << 0);
        delay();
        LPC_GPIO2->FIOCLR = (0x0F << 0);
        delay();
    }

    // Limpia la bandera de interrupción EINT1
    LPC_SC->EXTINT |= (1 << 1);
}

int main(void) {
    configLed0();
    configLed1();
    configIntGPIO_P0();
    configEint1();

    while (1) {
        __WFI(); // Entra en modo de espera hasta recibir una interrupción
    }
    return 0;
}
