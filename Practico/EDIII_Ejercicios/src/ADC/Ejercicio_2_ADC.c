/*Ejercicio 2: Adquisición Automática por Hardware con MAT 0.1 (Nivel Intermedio)
Objetivo: Dominar la sincronización entre periféricos por hardware sin intervención de la CPU durante el disparo (el clásico de la materia).
Planteo:
Diseñar un sistema de monitoreo para un sensor de temperatura (LM35 o potenciómetro) en el canal AD0.0.
Configurar la salida física MAT 0.1 del Timer 0 (P1.29) en modo Toggle para generar un flanco cada 500 ms (periodo total de 1s1Hz).
Configurar el ADC para que no use disparo por software, sino disparo por flanco descendente del evento de hardware MAT 0.1
(ADC_START_ON_MAT01, ADC_START_ON_FALLING).
La CPU debe permanecer en reposo hasta que el ADC termine su conversión automática y dispare la ADC_IRQHandler, donde se verificará
si la temperatura supera los 40 °C para activar la alarma.
Pasos de Razonamiento:
¿Cómo se configura el pin P1.29 en el PINSEL para que responda a la función MAT 0.1?
¿Por qué en este caso deshabilitamos la interrupción en el NVIC para el Timer0 pero dejamos el temporizador corriendo en bucle?
 * */

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include <stdbool.h>
#define rate_samples 10000

void configPin(){
	PINSEL_CFG_T cfgPinLed;

	cfgPinLed.port = PORT_0;
	cfgPinLed.pin = PIN_22;
	cfgPinLed.func=PINSEL_FUNC_00;
	cfgPinLed.mode = PINSEL_PULLDOWN;
	cfgPinLed.openDrain =DISABLE;

	PINSEL_ConfigPin(&cfgPinLed);
	GPIO_SetDir(PORT_0,(1<<22),GPIO_OUTPUT);
	GPIO_ClearPins(PORT_0,(1<<22));

	PINSEL_CFG_T cfgPin;

	cfgPin.port = PORT_1;
	cfgPin.pin = PIN_29;
	cfgPin.func=PINSEL_FUNC_11;
	cfgPin.mode = PINSEL_TRISTATE;
	cfgPin.openDrain =DISABLE;

	PINSEL_ConfigPin(&cfgPin);


}
void configTimer0(){
	TIM_TIMERCFG_T cfgTm0;

	cfgTm0.prescaleOpt = TIM_US;
	cfgTm0.prescaleValue=1 ;

	TIM_MATCHCFG_T cfgMatchTm0={
			.channel = TIM_MATCH_1,
			.intEn=DISABLE,
			.resetEn = ENABLE,
			.stopEn = ENABLE,
			.extOpt=TIM_TOGGLE,
			.matchValue=49999
	};



	TIM_Enable(LPC_TIM0);
	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
}

void configAdc(){
	ADC_PowerUp();
	ADC_PinConfig(ADC_CHANNEL_0);
	ADC_EdgeStartConfig(ADC_START_ON_FALLING);
	ADC_ChannelEnable(ADC_CHANNEL_0);
	ADC_IntEnable(ADC_INT_CH0);
	ADC_StartCmd(ADC_START_ON_MAT01);
	ADC_Init(rate_samples);
	NVIC_EnableIRQ(ADC_IRQn);
}
Bool signal_to_celcius(uint16_t signal){
	// 1. Extraemos el dato crudo de 12 bits (Cuentas)
	uint16_t lm35_raw = signal & 0xFFF;

	// 2. Convertimos las cuentas a milivoltios (mV)
	// Usamos uint32_t para evitar que la multiplicación se desborde (overflow)
	uint32_t lm35_mv = ((uint32_t)lm35_raw * 3300) / 4095;

	// 3. Convertimos los milivoltios a Grados Celsius (°C)
	uint32_t lm35_temp_c = lm35_mv / 10;

	if (lm35_temp_c >= 40){return true;}
	else {return false;}
}
void ADC_IRQHandler(){

	if (ADC_ChannelGetStatus(ADC_CHANNEL_0,ADC_DATA_DONE)==SET){
		uint16_t signal_temp = ADC_ChannelGetData(ADC_CHANNEL_0);
		if (signal_to_celcius(signal_temp)){
			  GPIO_SetPins(PORT_0, (1<<22));
		}else{ GPIO_ClearPins(PORT_0, (1<<22));}

	}
}

int main (){
	configPin();
	configTimer0();
	configAdc();
	while (1){
		__WFI();
	}
};
