/* Objetivo: Producir una señal cuadrada de 1 kHz (período 1ms, semiperiodo de 0.5ms=500s)
 * sobre el pin físico MAT 0.0 (P1.28) sin usar interrupciones ni recargar la CPU.
 * Contexto y Datos de Partida:
 * Con la base de tiempo de 1s por tick, el semiperiodo requiere 500s.
 * MR0=500-1=499.  */


#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
void confPin(void)
{
    PINSEL_CFG_T confPinselCfg;

    confPinselCfg.port      = PORT_1;
    confPinselCfg.pin       = PIN_28;
    confPinselCfg.func      = PINSEL_FUNC_11;   // Función alternativa 3: MAT0.0
    confPinselCfg.openDrain = DISABLE;
    confPinselCfg.mode      = PINSEL_TRISTATE;

    PINSEL_ConfigPin(&confPinselCfg);
}
void confgTimer0(void)
{
    // 1. Configuración de Base de Tiempo: 1 tick = 1 us
    TIM_TIMERCFG_T timerCfg;
    timerCfg.prescaleOpt   = TIM_US;
    timerCfg.prescaleValue = 1;
    TIM_InitTimer(LPC_TIM0, &timerCfg);

    // 2. Configuración del Match 0 para onda cuadrada autónoma
    TIM_MATCHCFG_T cfgTimerMatch;
    cfgTimerMatch.channel    = TIM_MATCH_0;
    cfgTimerMatch.intEn      = DISABLE;     // SIN interrupción a la CPU
    cfgTimerMatch.resetEn    = ENABLE;      // Resetea TC a 0 al coincidir
    cfgTimerMatch.stopEn     = DISABLE;     // Mantiene el conteo continuo
    cfgTimerMatch.extOpt     = TIM_TOGGLE;  // Conmuta el pin MAT0.0 automáticamente
    cfgTimerMatch.matchValue = 499;         // 500 us - 1 = 499 ticks

    TIM_ConfigMatch(LPC_TIM0, &cfgTimerMatch);

    // NO se habilita la interrupción en el NVIC

    // 3. Encendido del Timer
    TIM_Enable(LPC_TIM0);
}
// Handler de Interrupción del Timer 0
void TIMER0_IRQHandler(void)
{
    // Verifica si la interrupción fue causada por la coincidencia en MR0
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT) == SET)
    {
        // --- AQUÍ VA TU CÓDIGO A EJECUTAR CADA 50 ms ---
    	static uint32_t index ;

    	if (index % 2 == 0){

    		GPIO_SetPinState (PORT_1,PIN_28,SET);
    	}else {GPIO_SetPinState (PORT_1,PIN_28,RESET);}
        // ¡OBLIGATORIO! Limpia la bandera de interrupción en el periférico
        TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
    }
}
int main (){
	confPin();
	confgTimer0 ();
	while(1){
		__WFI();
	}
};






