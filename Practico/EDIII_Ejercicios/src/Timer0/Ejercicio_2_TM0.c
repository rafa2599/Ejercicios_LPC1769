/*Ejercicio 2: Temporizador Periódico por Interrupción
Objetivo: Generar una interrupción periódica cada 250 ms sin modificar pines por hardware.
Consigna: Configurar la base de tiempo del Timer 0 para que incremente el contador TC cada 1 µs (PR = 24 a 25 MHz).
Programar MR0 en 250.000 y configurar el registro MCR para que al haber coincidencia:
Se genere una interrupción (MR0I = 1).
Se reinicie el contador TC a cero (MR0R = 1).
El temporizador siga contando (MR0S = 0).
En la ISR: Conmutar el estado de un LED en P0.22 mediante GPIO y limpiar la bandera en IR.
 * */

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include <stdio.h>
#include <stdbool.h>



volatile Bool intFlag = false;

void configLed(){
	PINSEL_CFG_T cfgLed;
	cfgLed.port = PORT_0;
	cfgLed.pin=PIN_22;
	cfgLed.func=PINSEL_FUNC_00;
	cfgLed.mode=PINSEL_PULLDOWN;
	cfgLed.openDrain = DISABLE;
	PINSEL_ConfigPin(&cfgLed);
	GPIO_SetDir(PORT_0,(1<<22),GPIO_OUTPUT);
	GPIO_ClearPins(PORT_0,(1<<22));

}
void configTimer(){
	TIM_TIMERCFG_T cfgTm0;
	cfgTm0.prescaleOpt = TIM_US;
	cfgTm0.prescaleValue =1; //1 US

	TIM_MATCHCFG_T cfgMatchTm0;

	cfgMatchTm0.channel = TIM_MATCH_0;
	cfgMatchTm0.intEn = ENABLE;
	cfgMatchTm0.stopEn = DISABLE;
	cfgMatchTm0.resetEn = ENABLE;
	cfgMatchTm0.extOpt = TIM_NOTHING;
	cfgMatchTm0.matchValue = 249999;

	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
	TIM_Enable(LPC_TIM0);
	NVIC_EnableIRQ(TIMER0_IRQn);
}


void Timer0_IRQHandler(){

	if (TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT)==SET){

		GPIO_TogglePins(PORT_0,(1<<22));
		TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);

	}
}

int main (){
	configLed();
	configTimer();

	while(1){
		__WFI();
	}

	return 0;
};
