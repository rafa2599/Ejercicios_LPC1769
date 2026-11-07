/*===============================================================================
* @file
Ejercicio_6_2_2.c
*
* @brief
Se requiere monitorizar la temperatura ambiente mediante dos sensores analógicos LM35 conectados a los canales AD0.0 y AD0.1 del ADC.
*
Realizar el muestreo periódico de ambas señales y detectar el sobrepaso de un umbral crítico de temperatura de 60 °C mediante
*
alarma técnica visual de prioridad alta, conformada por un LED rojo en el pin P0.22.
*
* @details - Las señales de los sensores LM35 se acondicionan mediante
*
amplificadores operacionales configurados para maximizar el aprovechamiento del rango dinámico del ADC.

- El acondicionamiento adapta la tensión generada por los sensores al rango de entrada del LPC1769 (0 V a 3.3 V),
*
incrementando la resolución efectiva de la medición y mejorando la inmunidad al ruido durante la adquisición.
*
- Sensibilidad LM35: lineal de 10 mV/°C.
*
- Base de tiempo de adquisición: 1 muestra por segundo (1 Hz) mediante el disparo por hardware de MAT0.1 del Timer 0.
*
- Al finalizar AD0.0, la ISR inicia por software la conversión del canal AD0.1.
*
- Implementación en alto nivel mediante drivers (NXP LPC17xx).
*
* @author Ing. E. Migliore
*
* @version 1.0
*=============================================================================*/
#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"



#define   LM35_FACTOR 10
#define  FREC_ADC 1200000
volatile static uint16_t channel0Data ;
volatile static uint16_t channel1Data ;

void configPin(){
	PINSEL_CFG_T cfgLed ={
			.port = PORT_0,
			.pin = PIN_22,
			.func = PINSEL_FUNC_00,
			.mode = PINSEL_PULLDOWN,
			.openDrain = DISABLE
	};
	PINSEL_ConfigPin(&cfgLed);
	GPIO_SetDir(PORT_0,(1>22),GPIO_OUTPUT);
	GPIO_ClearPins(PORT_0,(1>22));


	PINSEL_CFG_T cfgAdc0={
			.port=PORT_0,
			.pin =PIN_23,
			.func = PINSEL_FUNC_01,
			.mode = PINSEL_TRISTATE,
			.openDrain= DISABLE

	};
	PINSEL_ConfigPin(&cfgAdc0);

	PINSEL_CFG_T cfgAdc1={
			.port=PORT_0,
			.pin =PIN_24,
			.func = PINSEL_FUNC_01,
			.mode = PINSEL_TRISTATE,
			.openDrain= DISABLE

	};
	PINSEL_ConfigPin(&cfgAdc1);

}

void configTimer(){
	TIM_TIMERCFG_T cfgTm0 = {TIM_US,1};

	TIM_MATCHCFG_T cfgMatchTm0 = {
			.channel= TIM_MATCH_1,
			.intEn = DISABLE,
			.stopEn = DISABLE,
			.resetEn = ENABLE,
			.extOpt = TIM_TOGGLE ,
			.matchValue = 999999
	};
	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch (LPC_TIM0,&cfgMatchTm0);
}

void configAdc(){
	// Energizamos el pin
	ADC_PowerUp();
	//Deshabilitamos el modo BURST
	ADC_BurstDisable();
	// Configuramos los pines de cada canal y habilitamos el canal 0 solamete
	ADC_PinConfig(ADC_CHANNEL_0);
	ADC_PinConfig(ADC_CHANNEL_1);

	ADC_ChannelEnable(ADC_CHANNEL_0);
	//Habilitamos las interrupciones
	ADC_IntEnable(ADC_CHANNEL_0);
	// Configuramos el modo de disparo del ADC
	ADC_StartCmd (ADC_START_ON_MAT01);
	ADC_EdgeStartConfig(ADC_START_ON_RISING);
	//Habilitamos las ISR
	NVIC_EnableIRQ(ADC_IRQn);
	//Inicializamos el ADC
	ADC_Init(FREC_ADC);
}

uint16_t  cuentas_to_celcius (uint16_t param){

	uint16_t lm35_raw = param & 0X0FFF;

	uint16_t lm35_mv = (lm35_raw *3300)/4095;

	uint16_t lm35_c = lm35_mv / LM35_FACTOR;

	return lm35_c ;

}

void set_alarm(uint16_t temp){

	if (temp >= 60){
		GPIO_SetPins(PORT_0,(1>22));
	}else{
		GPIO_ClearPins(PORT_0,(1>22));}
}


void TIMER0_IRQHandler(){

	if (ADC_ChannelGetStatus(0,ADC_DATA_DONE)==SET){

		channel0Data = cuentas_to_celcius (ADC_ChannelGetData (0));

		set_alarm(channel0Data);

		ADC_ChannelDisable(0);
		ADC_ChannelEnable(1);
		ADC_IntDisable(0);
		ADC_IntEnable(1);
	}
	else if (ADC_ChannelGetStatus(0,ADC_DATA_DONE)==SET){

		channel1Data = cuentas_to_celcius (ADC_ChannelGetData (1));

		set_alarm(channel1Data);

		ADC_ChannelDisable(1);
		ADC_ChannelEnable(0);
		ADC_IntDisable(1);
		ADC_IntEnable(0);
	}
}


int main (){
	configPin();
	configTimer();
	configAdc();

	while (1){
		__WFI();
	}
	return 0;

};
