/*
 * Ejercicio 4: Temporización de Disparo Único ( One-Shot )
Objetivo: Controlar la detención automática del temporizador por hardware.
Consigna: Programar el Timer 0 para que mida un intervalo exacto de 3 segundos tras ser iniciado. Al coincidir TC == MR0:
Debe solicitar una interrupción (MR0I = 1).
Debe reiniciar TC a cero (MR0R = 1).
Debe detener automáticamente el contador (MR0S = 1) para no seguir contando hasta nuevo aviso.
 *
 */
#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

void configTimer0(){
	TIM_TIMERCFG_T cfgTm0;
	cfgTm0.prescaleOpt = TIM_US;
	cfgTm0.prescaleValue = 1;

	TIM_MATCHCFG_T cfgMatchTm0;
	cfgMatchTm0.channel= ENABLE;
	cfgMatchTm0.intEn= ENABLE;
	cfgMatchTm0.stopEn= ENABLE;
	cfgMatchTm0.resetEn = ENABLE;
	cfgMatchTm0.extOpt = TIM_NOTHING;
	cfgMatchTm0.matchValue = 2999999;

	TIM_Enable(LPC_TIM0);
	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
	TIM_ClearIntPending(LPC_TIM0,TIM_MATCH_0);

	NVIC_EnableIRQ(TIMER0_IRQn);

}
void TIMER0_IRQHandler(){

	if ( TIM_GetIntStatus(LPC_TIM0,TIM_MATCH_0)==SET){

	}
}
