#include "LPC17xx.h"

#define MASK_DISPLAY 0x7F // Pines P0.0 a P0.6 para los segmentos A-G

const uint8_t symbols[16] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x67, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71  // F
};

// Retardo por software para el antirrebote
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 20000; i++) {
        __NOP();
    }
}

void configPinsDisplay(void) {
    // Configuramos P0.0 a P0.6 como GPIO (función 00 en PINSEL0)
    LPC_PINCON->PINSEL0 &= ~(0x3FFF);

    // Salidas para los 7 segmentos
    LPC_GPIO0->FIODIR |= MASK_DISPLAY;
    LPC_GPIO0->FIOCLR = MASK_DISPLAY; // Apaga segmentos iniciales

    // Configuración del pin de alimentación/habilitación del display (P0.22)
    LPC_PINCON->PINSEL1 &= ~(3 << 12);
    LPC_GPIO0->FIODIR |= (1 << 22);
    LPC_GPIO0->FIOSET = (1 << 22); // Habilita el display
}

void configCounter(void) {
    // Configura P2.0 como GPIO (función 00 en PINSEL4)
    LPC_PINCON->PINSEL4 &= ~(3 << 0);

    // Resistencia Pull-Down en P2.0 (PINMODE4 = 11)
    LPC_PINCON->PINMODE4 |= (3 << 0);

    // Configura P2.0 como entrada
    LPC_GPIO2->FIODIR &= ~(1 << 0);
}

void display(uint8_t valor) {
    // Limpia los 7 segmentos del Puerto 0
    LPC_GPIO0->FIOCLR = MASK_DISPLAY;

    // Escribe el patrón correspondiente al dígito
    LPC_GPIO0->FIOSET = (symbols[valor] & MASK_DISPLAY);
}

int main(void) {
    // Inicialización de periféricos
    configPinsDisplay();
    configCounter();

    uint8_t counter = 0;
    display(counter); // Muestra el '0' inicial

    while (1) {
        // Verifica si se presiona el botón en P2.0 (1 lógico)
        if (LPC_GPIO2->FIOPIN & (1 << 0)) {

            // 1. Primer retardo de antirrebote
            delay_ms(20);

            // Confirma que sigue presionado tras el retardo
            if (LPC_GPIO2->FIOPIN & (1 << 0)) {

                // Incrementa el contador (módulo 16: vuelve a 0 al pasar de F)
                counter = (counter + 1) % 16;
                display(counter);

                // 2. Espera a que el usuario suelte el botón (Detección de flanco)
                while (LPC_GPIO2->FIOPIN & (1 << 0));

                // 3. Antirrebote al soltar el pulsador
                delay_ms(20);
            }
        }
    }
    return 0;
}
