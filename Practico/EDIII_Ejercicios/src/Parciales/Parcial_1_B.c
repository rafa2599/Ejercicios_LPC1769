/** @brief
Diseñar e implementar en C un generador de dos señales cuadradas de 50 Hz por software utilizando el temporizador SysTick, con control de
*
desfasaje (phase shift) dinámico entre ambas señales mediante la interrupción externa EINT0, por flanco ascendente.
*
* @details:
* - 1. Generar por el pin P0.15 la Señal A (referencia) como una onda

cuadrada con un período de 20 ms (50 Hz) y un ciclo de trabajo (duty cycle) del 50% (10 ms en ALTO, 10 ms en BAJO).

- 2. Generar por el pin P0.16 la Señal B, idéntica en frecuencia y duty cycle, pero con un desfasaje temporal respecto a la Señal A.

- 3. Mediante la pulsación de un switch conectado a EINT0,
rotar cíclicamente el desfasaje de la Señal B en 4 estados: 0° (0 ms), 90° (5 ms), 180° (10 ms) y 270° (15 ms).

- 4. Configurar el SysTick con una base de tiempo fija de 1 ms, considerando CCLK = 100 MHz.
 * */

#include "LPC17xx.h"
#include <stdbool.h>

typedef enum {
    A, // 0°   (0 ms de desfasaje)
    B, // 90°  (5 ms de desfasaje)
    C, // 180° (10 ms de desfasaje)
    D  // 270° (15 ms de desfasaje)
} STATES;

#define N_TICKS 99999 // 1 ms @ 100 MHz CCLK

static volatile uint8_t counter_ticks = 0;
static volatile STATES estado_actual = A;

// Tabla de offset en milisegundos correspondiente a cada estado
static const uint8_t offsets[4] = {0, 5, 10, 15};

void configPines(void) {
    // Configura P0.15 como GPIO (PINSEL0 bits 31:30 en 00)
    LPC_PINCON->PINSEL0 &= ~(3 << 30);

    // Configura P0.16 como GPIO (PINSEL1 bits 1:0 en 00)
    LPC_PINCON->PINSEL1 &= ~(3 << 0);

    // Configura P0.15 y P0.16 como salidas digitales
    LPC_GPIO0->FIODIR |= (1 << 15) | (1 << 16);
    LPC_GPIO0->FIOMASK &= ~((1 << 15) | (1 << 16));
    LPC_GPIO0->FIOCLR = (1 << 15) | (1 << 16);

    // Configura P2.10 como EINT0 (PINSEL4 bits 21:20 en 01)
    LPC_PINCON->PINSEL4 &= ~(3 << 20);
    LPC_PINCON->PINSEL4 |=  (1 << 20);
}

void configSysTick(void) {
    SysTick->LOAD = N_TICKS;
    SysTick->VAL = 0;
    SysTick->CTRL = (1 << 0) | (1 << 1) | (1 << 2); // ENABLE, TICKINT, CLKSOURCE
}

void configEint0(void) {
    LPC_SC->EXTMODE |= (1 << 0);   // Sensible por flanco
    LPC_SC->EXTPOLAR |= (1 << 0);  // Flanco ascendente
    LPC_SC->EXTINT |= (1 << 0);    // Limpia bandera pendiente

    NVIC_SetPriority(EINT0_IRQn, 1);
    NVIC_EnableIRQ(EINT0_IRQn);
}

void EINT0_IRQHandler(void) {
    // Rotación cíclica de estados: A -> B -> C -> D -> A
    estado_actual = (STATES)((estado_actual + 1) % 4);

    // Limpia la bandera de interrupción externa EINT0
    LPC_SC->EXTINT = (1 << 0);
}

void SysTick_IRQHandler(void) {
    // -------------------------------------------------------------
    // 1. Generación de Señal A (Referencia en P0.15)
    // -------------------------------------------------------------
    if (counter_ticks < 10) {
        LPC_GPIO0->FIOSET = (1 << 15); // ALTO durante los primeros 10 ms
    } else {
        LPC_GPIO0->FIOCLR = (1 << 15); // BAJO durante los siguientes 10 ms
    }

    // -------------------------------------------------------------
    // 2. Generación de Señal B (P0.16 con desfasaje dinámico)
    // -------------------------------------------------------------
    uint8_t shift = offsets[estado_actual]; // 0, 5, 10 o 15 ms
    uint8_t t_b = (counter_ticks >= shift) ? (counter_ticks - shift) : (counter_ticks + 20 - shift);

    if (t_b < 10) {
        LPC_GPIO0->FIOSET = (1 << 16); // ALTO
    } else {
        LPC_GPIO0->FIOCLR = (1 << 16); // BAJO
    }

    // -------------------------------------------------------------
    // 3. Incremento del contador base (período total = 20 ms)
    // -------------------------------------------------------------
    counter_ticks++;
    if (counter_ticks >= 20) {
        counter_ticks = 0;
    }
}

int main(void) {
    configPines();
    configEint0();
    configSysTick();

    while (1) {
        __WFI(); // Espera interrupciones en modo de bajo consumo
    }

    return 0;
}
