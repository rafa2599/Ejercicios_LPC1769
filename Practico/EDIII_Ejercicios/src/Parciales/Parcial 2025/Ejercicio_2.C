/*
 * Por un pin del ADC del microcontrolador LPC1769 ingresa una tensión de rango dinámico 0 a 3,3[v]
 * proveniente de un sensor de temperatura. Debido a la baja tasa de variación de la señal, se pide tomar una muestra cada 30[s].
 * Pasados los 2[min] se debe promediar las últimas 4 muestras y en función de este valor,
 * tomar una decisión sobre una salida digital de la placa:

Si el valor es <1 [V] colocar la salida en 0 (0[V]).
Si el valor es >= 1[V] y <=2[V] sacar una señal intermitente de  F= ????
Si el valor es > 2[V] colocar la salida en 1 (3,3[V]).
 *
 * */

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
volatile static uint16_t buffer [4];
void configLed(){}
void configTimer(){

	TIM_TIMERCFG_T cfgTm0 ={TIM_US,1000}; // 1ms
	TIM_MATCHCFG_T cfgMatchTm0 ={
			.channel = TIM_MATCH_1,
			.intEn = ENABLE,
			.stopEn =DISABLE,
			.resetEn =ENABLE,
			.extOpt = TIM_TOGGLE,
			.matchValue = 29000
	};
	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
	NVIC_EnableIRQ(TIMER0_IRQn);
	TIM_Enable(LPC_TIM0);
}

void configAdc(){
	//Energizo el ADC
	ADC_PowerUp();
	//Configuro el pin asociado con el canal 0 del adc
	ADC_PinConfig(ADC_CHANNEL_0);
	//Habilito el canal
	ADC_ChannelEnable(ADC_CHANNEL_0);
	//Configuro el modo de disparo
	ADC_EdgeStartConfig(ADC_START_ON_RISING);
	ADC_StartCmd(ADC_START_ON_MAT01);
}
uint16_t cuentas_to_volts (){
	static uint16_t promedio = 0;
	for (volatile uint16_t i=0 ; i<4 ;i++){

		uint16_t lm35_raw=( buffer[i]& 0XFFF);
		uint16_t lm35_mv = (lm35_raw *3300)/4095;

		promedio += lm35_mv;

	}
	promedio = promedio/4;
	return promedio ;
}
void setAlarm(uint16_t param){

	if (param < 1){}
	else if (para > 2){}
	else {}
}
void TIMER0_IRQHandler(){
	static uint8_t num_muestras = 0;

	if (ADC_ChannelGetStatus (ADC_CHANNEL_0,ADC_DATA_DONE)==SET && num_muestras<=2){
		uint16_t channelData = ADC_ChannelGetData(ADC_CHANNEL_0);
		buffer[num_muestras]= channelData;
		num_muestras ++;

	}
	else if (ADC_ChannelGetStatus (ADC_CHANNEL_0,ADC_DATA_DONE)==SET && num_muestras==3){
		//guardo la ultima muestra
		uint16_t channelData = ADC_ChannelGetData(ADC_CHANNEL_0);
		buffer[num_muestras]= channelData;
		// Saco un promedio
		uint16_t valor = cuentas_to_volts ();
		//Seteo la alarma
		setAlarm(valor);
		//reseteo el numero de muestras
		num_muestras = 0;



	}
}
