/*Ejercicio 5: Generador de Onda Cuadrada Autónomo (Modo TOGGLE )
Objetivo: Producir una señal de 1 kHz en el pin MAT 0.0 (P1.28) sin consumir ciclos de CPU.
Consigna:
Configurar el multiplexor de pines (PINSEL3) para asignar la función MAT 0.0 al pin P1.28.
Configurar MR0 para un semiperiodo de 500 µs con reinicio automático (MR0R = 1).
En el registro EMR (o mediante ExtMatchOutputType), seleccionar la modalidad TOGGLE (conmutación) para el canal 0 (EMC 0 = 11).
Deshabilitar la interrupción por software (MR0I = 0) para verificar que la onda sale de forma 100% autónoma. */

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"


void configTimer(){
	TIM_TIMERCFG_T cfgTm0;

	cfgTm0.prescaleOpt= TIM_US;
	cfgTm0.prescaleValue = 1;

	TIM_MATCHCFG_T cfgMatchTm0;

	cfgMatchTm0.channel=TIM_MATCH_0;
	cfgMatchTm0.intEn = DISABLE;
	cfgMatchTm0.resetEn = ENABLE;
	cfgMatchTm0.stopEn = DISABLE;
	cfgMatchTm0.extOpt = TIM_TOGGLE;
	cfgMatchTm0.matchValue = 499;

	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
	TIM_Enable(LPC_TIM0);

}
void configPin(){
	PINSEL_CFG_T cfgPin;

	cfgPin.port = PORT_1;
	cfgPin.pin = PIN_28;
	cfgPin.func=PINSEL_FUNC_11;
	cfgPin.mode = PINSEL_TRISTATE;
	cfgPin.openDrain =DISABLE;

	PINSEL_ConfigPin(&cfgPin);
	GPIO_SetDir(PORT_0,(1<<28),GPIO_OUTPUT);

}
int main (){
	configPin();
	configTimer();
	while(1){
		__WFI();
	}
	return 0;
}
