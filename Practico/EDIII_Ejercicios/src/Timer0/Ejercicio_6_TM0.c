/*
 * Ejercicio 6: Generación de un Pulso con Ancho Determinado ( HIGH y LOW )
Objetivo: Combinar dos canales de coincidencia para formar un pulso asimétrico por hardware.
Consigna:
Configurar el pin P1.28 ( MAT 0.0 ).
Al llegar TC == MR0 (a los 10 ms), configurar EMR para que fuerce la salida a nivel alto (SET / HIGH)[1][2].
Al llegar TC == MR1 (a los 40 ms), configurar EMR para que fuerce la salida a nivel bajo (CLEAR / LOW) y reinicie TC a cero (MR1R = 1).
Resultado: Generarás una señal periódica con 30 ms en nivel alto y 10 ms en nivel bajo.
 *
 * */
#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"


void configPin(){
	PINSEL_CFG_T cfgPin;

	cfgPin.port = PORT_1;
	cfgPin.pin = PIN_28;
	cfgPin.func=PINSEL_FUNC_11;
	cfgPin.mode = PINSEL_TRISTATE;
	cfgPin.openDrain =DISABLE;

	PINSEL_ConfigPin(&cfgPin);


}
void configTimer0(){
	TIM_TIMERCFG_T cfgTm0;

	cfgTm0.prescaleOpt= TIM_US;
	cfgTm0.prescaleValue = 1;

	TIM_MATCHCFG_T cfgMatchTm0;

	cfgMatchTm0.channel=TIM_MATCH_0;
	cfgMatchTm0.intEn = DISABLE;
	cfgMatchTm0.resetEn = ENABLE;
	cfgMatchTm0.stopEn = DISABLE;
	cfgMatchTm0.extOpt = TIM_HIGH;
	cfgMatchTm0.matchValue = 9999;
	TIM_Enable(LPC_TIM0);
	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
}
void configTimer1(){
	TIM_TIMERCFG_T cfgTm1;

	cfgTm1.prescaleOpt= TIM_US;
	cfgTm1.prescaleValue = 1;

	TIM_MATCHCFG_T cfgMatchTm1;

	cfgMatchTm1.channel=TIM_MATCH_1;
	cfgMatchTm1.intEn = ENABLE;
	cfgMatchTm1.resetEn = ENABLE;
	cfgMatchTm1.stopEn = ENABLE;
	cfgMatchTm1.extOpt = TIM_NOTHING;
	cfgMatchTm1.matchValue = 39999;


	TIM_InitTimer(LPC_TIM0,&cfgTm1);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm1);
	TIM_ClearIntPending(LPC_TIM0,TIM_MATCH_1);

	NVIC_EnableIRQ(TIMER0_IRQn);
}
void TIMER0_IRQHandler(){

	if (TIM_GetIntStatus(LPC_TIM0,TIM_MATCH_1)==SET){

		LPC_TIM0->EMR &= ~(1 << 0);
		TIM_ClearIntPending(LPC_TIM0,TIM_MATCH_1);

	}
}

int main (){
	configPin();
	configTimer0();
	configTimer1();
	while(1){
		__WFI();
	}
}
