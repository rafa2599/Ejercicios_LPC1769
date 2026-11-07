/*Ejercicio 2: Temporización Periódica por Interrupción (Modo Match)
Objetivo: Configurar el canal de coincidencia MR0 para generar una interrupción al procesador cada 50 milisegundos (50 ms)
y reiniciar automáticamente el contador.
Contexto y Datos de Partida:
Con la base de tiempo del Ejercicio 1 (PR=24), cada tick de TC equivale a 1s.
Tiempo de coincidencia deseado: tmatch=50ms=50.000s.


Contexto y Datos de Partida:
Frecuencia del reloj principal de CPU: CCLK=100MHz.
Reloj del periférico por defecto: PCLK=CCLK4=25MHz4.
Queremos que el contador de tiempo TC se incrementa cada 1s
 * */
#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"


void initTimer0_Match0(void)
{
    // 1. Configuración de la Base de Tiempo (Ejercicio 1)
    TIM_TIMERCFG_T timerCfg;
    timerCfg.prescaleOpt = TIM_US;
    timerCfg.prescaleValue = 1;        // 1 tick = 1 us
    TIM_InitTimer(LPC_TIM0, &timerCfg);

    // 2. Configuración del Match 0
    TIM_MATCHCFG_T matchCfg;
    matchCfg.channel    = TIM_MATCH_0; // Usamos el canal MR0
    matchCfg.intEn      = ENABLE;      // Genera interrupción al coincidir
    matchCfg.resetEn    = ENABLE;      // Reinicia TC a 0 automáticamente
    matchCfg.stopEn     = DISABLE;     // Sigue contando indefinidamente
    matchCfg.extOpt     = TIM_NOTHING; // Sin acción en pines externos
    matchCfg.matchValue = 49999;       // 50.000 us - 1 = 50 ms

    // Carga la configuración en los registros de hardware del Timer 0
    TIM_ConfigMatch(LPC_TIM0, &matchCfg);

    // 3. Habilita la interrupción del Timer 0 en el NVIC del Cortex-M3
    NVIC_EnableIRQ(TIMER0_IRQn);

    // 4. Arranca el temporizador
    TIM_Enable(LPC_TIM0);
}

// Handler de Interrupción del Timer 0
void TIMER0_IRQHandler(void)
{
    // Verifica si la interrupción fue causada por la coincidencia en MR0
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT) == SET)
    {
        // --- AQUÍ VA TU CÓDIGO A EJECUTAR CADA 50 ms ---

        // ¡OBLIGATORIO! Limpia la bandera de interrupción en el periférico
        TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
    }
}
