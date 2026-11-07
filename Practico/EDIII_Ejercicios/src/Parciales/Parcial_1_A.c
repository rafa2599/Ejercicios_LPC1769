/*Utilizando SysTick e interrupciones externas escribir un código en C que cuente indefinidamente de 0 a 9. Un pulsador conectado a EINT0
*reiniciará la cuenta de 0 y se mantendrá en ese valor mientras el pulsador se encuentre presionado. Un pulsador conectado a EINT1
permitirá detener o continuar la cuenta cada vez que sea presionado.Un pulsador conectado a EINT2 permitirá modificar la velocidad de
incremento del contador. En ese sentido, cada vez que se presione ese pulsador el contador pasará a incrementar su cuenta de cada 1 segundo
a cada 1 milisegundo y viceversa.
*
* @details - Salida: P2.0 a P2.6 para los segmentos 'a' hasta 'g'.
*
- EINT0 (P2.10): Resetea y congela la cuenta en 0 mientras se presiona.
*
- EINT1 (P2.11): Alterna entre pausa y ejecución.
*
- EINT2 (P2.12): Alterna la velocidad entre 1 s y 1 ms.
*
- Frecuencia de reloj (CCLK): 80 MHz.
 * */
#include "LPC17xx.h"
#include <stdbool.h>

// 1000 interrupciones de 1 ms = 1 s ; 1 interrupción = 1 ms
const uint32_t seg_delay = 1000;
const uint32_t mili_seg_delay = 1;

const uint8_t symbols[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x67  // 9
};

// Variables globales de estado
volatile uint8_t count = 0;
volatile bool is_paused = false;
volatile uint32_t current_delay = 1000; // Inicia en 1 segundo (seg_delay)
volatile uint32_t tick_counter = 0;

void showDisplay(uint8_t digit);

void configPinDisplay() {
    // Configura P2.0 a P2.6 como GPIO (bits 0 a 13 en 00)
    LPC_PINCON->PINSEL4 &= ~(0x3FFF << 0);

    // P2.0 a P2.6 como salidas digitales
    LPC_GPIO2->FIODIR |= (0x7F << 0);
    LPC_GPIO2->FIOMASK &= ~(0x7F << 0);
    LPC_GPIO2->FIOCLR = (0x7F << 0);
}

void configPinsExt() {
    // EINT0 (P2.10): bits 21:20 = 01
    LPC_PINCON->PINSEL4 &= ~(3 << 20);
    LPC_PINCON->PINSEL4 |=  (1 << 20);

    // EINT1 (P2.11): bits 23:22 = 01
    LPC_PINCON->PINSEL4 &= ~(3 << 22);
    LPC_PINCON->PINSEL4 |=  (1 << 22);

    // EINT2 (P2.12): bits 25:24 = 01
    LPC_PINCON->PINSEL4 &= ~(3 << 24);
    LPC_PINCON->PINSEL4 |=  (1 << 24);
}

void configEint0() {
    // EINT0 sensible por nivel alto (para detectar mantención)
    LPC_SC->EXTMODE &= ~(1 << 0);  // 0 = Por Nivel
    LPC_SC->EXTPOLAR |= (1 << 0);  // 1 = Alto
    NVIC_SetPriority(EINT0_IRQn, 1);
    NVIC_EnableIRQ(EINT0_IRQn);
}

void configEint1() {
    // EINT1 por flanco ascendente
    LPC_SC->EXTMODE |= (1 << 1);   // 1 = Por Flanco
    LPC_SC->EXTPOLAR |= (1 << 1);  // 1 = Ascendente
    NVIC_SetPriority(EINT1_IRQn, 2);
    NVIC_EnableIRQ(EINT1_IRQn);
}

void configEint2() {
    // EINT2 por flanco ascendente
    LPC_SC->EXTMODE |= (1 << 2);   // 1 = Por Flanco
    LPC_SC->EXTPOLAR |= (1 << 2);  // 1 = Ascendente
    NVIC_SetPriority(EINT2_IRQn, 3);
    NVIC_EnableIRQ(EINT2_IRQn);
}

void configSystick() {
    // SysTick interrumpe cada 1 ms (80.000 ciclos a 80 MHz)
    SysTick->LOAD = (80000 - 1);
    SysTick->VAL = 0;
    SysTick->CTRL = (1 << 0) | (1 << 1) | (1 << 2); // ENABLE, TICKINT, CLKSOURCE (CCLK)
}
void EINT0_IRQHandler() {
    // Resetea y mantiene en 0 la cuenta
    count = 0;
    tick_counter = 0;
    showDisplay(count);

    LPC_SC->EXTINT = (1 << 0); // Limpia bandera EINT0
}

void EINT1_IRQHandler() {
    if (LPC_SC->EXTINT & (1 << 1)) {
        is_paused = !is_paused; // Alterna entre pausa y ejecución
        LPC_SC->EXTINT = (1 << 1); // Limpia bandera EINT1
    }
}

void EINT2_IRQHandler() {
    if (LPC_SC->EXTINT & (1 << 2)) {
        // Alterna la velocidad entre 1 s (1000 ms) y 1 ms (1 ms)
        if (current_delay == seg_delay) {
            current_delay = mili_seg_delay;
        } else {
            current_delay = seg_delay;
        }
        LPC_SC->EXTINT = (1 << 2); // Limpia bandera EINT2
    }
}

void SysTick_IRQHandler() {
    // Si EINT0 está presionado (P2.10 en ALTO), congela el contador en 0
    if (LPC_GPIO2->FIOPIN & (1 << 10)) {
        count = 0;
        tick_counter = 0;
        showDisplay(count);
        return;
    }

    // Si no está en pausa, incrementa tiempo
    if (!is_paused) {
        tick_counter++;
        if (tick_counter >= current_delay) {
            tick_counter = 0;
            count = (count + 1) % 10; // Cuenta de 0 a 9
            showDisplay(count);
        }
    }
}

void showDisplay(uint8_t digit) {
    LPC_GPIO2->FIOCLR = (0x7F << 0);          // Apaga segmentos (P2.0 - P2.6)
    LPC_GPIO2->FIOSET = (symbols[digit] & 0x7F); // Enciende según tabla
}

int main(void) {
    configPinDisplay();
    configPinsExt();
    configEint0();
    configEint1();
    configEint2();
    configSystick();

    showDisplay(0);

    while (1) {
        __WFI();
    }
}
