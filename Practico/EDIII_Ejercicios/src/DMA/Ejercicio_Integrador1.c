#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_exti.h"

void configPin(){
	PINSEL_CFG_T configPin;
	configPin.port = PORT_2;
	configPin.pin= PIN_10;
	configPin.func = PINSEL_FUNC_01;
	configPin.mode = PINSEL_PULLDOWN;
	configPin.openDrain= DISABLE;
	PINSEL_ConfigPin(&configPin);
	GPIO_ClearPins(PORT_2,1<<10);
	GPIO_SetDir(PORT_2,PIN_10,GPIO_INPUT);



}
void configExt0(){
	EXTI_CFG_T configExt0;
	configExt0.line =EXTI_EINT0;
	configExt0.mode = EXTI_EDGE_SENSITIVE;
	configExt0.polarity = EXTI_HIGH_ACTIVE;
	EXTI_Init();
	EXTI_ConfigEnable (&configExt0);
	NVIC_EnableIRQ(EINT0_IRQn);

}

void configTimer0(){
	TIM_TIMERCFG_T  configTm0;
	configTm0.prescaleOpt =TIM_US;
	configTm0.prescaleValue = 1;
	TIM_MATCHCFG_T cofigMatch;

	cofigMatch.channel =TIM_MATCH_0;
	cofigMatch.intEn = ENABLE;
	cofigMatch.stopEn = ENABLE;
	cofigMatch.resetEn = DISABLE;
	cofigMatch.extOpt = TIM_NOTHING;
	cofigMatch.matchValue=500000;

	TIM_InitTimer(LPC_TIM0,&configTm0);
	// 4\. Habilitar la interrupción del Timer 0 en el NVIC
	NVIC_EnableIRQ(TIMER0_IRQn);
	// 5\. Arrancar el temporizador
	//TIM_Cmd(LPC_TIM0, ENABLE);
}
void configAdc(){
	//Configuramos el puerto P0.23 PARA TOMAR LA SEÑAL ANALOGICA DEL ADC
	PINSEL_CFG_T configPortAdc;
	configPortAdc.port =PORT_0;
	configPortAdc.pin = PIN_23;
	configPortAdc.func = PINSEL_FUNC_01;
	configPortAdc.mode = PINSEL_TRISTATE;
	configPortAdc.openDrain = DISABLE;
	PINSEL_ConfigPin(&configPortAdc);
	GPIO_SetDir(PORT_0,PIN_23,GPIO_INPUT);
	// Configuramos el ADC
	ADC_PowerUp();
	ADC_PinConfig(ADC_CHANNEL_0);
	ADC_ChannelEnable(ADC_CHANNEL_0);
	//ADC_Init();
	//ADC_BrustDisable();
}
void EINT0_IRQHandler(){

}
void TIMER0_IRQHandler(){
	// Verificar si la interrupción fue por la coincidencia MR0 (1 segundo)
	if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT) == SET) {
		// Habilitar / Iniciar la conversión en el Canal 0 del ADC por software
		ADC_StartCmd(ADC_START_ON_RISING);
		// Limpiar la bandera de interrupción del Timer 0
		TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
	}
}
void confAdc(){}

