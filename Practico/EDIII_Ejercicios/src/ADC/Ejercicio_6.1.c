/*===============================================================================
* @file
Ejercicio_6_1_2.c
*
* @brief
Se requiere monitorizar la temperatura ambiente mediante un sensor analógico LM35 conectado al canal AD0.0 del ADC..
*
Realizar el muestreo periódico de la señal y la detección de sobrepaso de un umbral crítico de temperatura de 60 °C mediante una
*
alarma técnica visual de prioridad alta, conformada por un LED rojo en el pin P0.22.
*
* @details - Las señales de los sensores LM35 se acondicionan mediante amplificadores operacionales configurados para maximizar el
*
aprovechamiento del rango dinámico del ADC.El acondicionamiento adapta la tensión generada por los sensores al rango de entrada del LPC1769 (0 V a 3.3 V),
*
incrementando la resolución efectiva de la medición y mejorando la inmunidad al ruido durante la adquisición.
*
- Base de tiempo de adquisición: 1 muestra por segundo (1 Hz) mediante el disparo por hardware de MAT0.1 del Timer 0.
*
- Implementación en alto nivel mediante drivers (NXP LPC17xx).
*
* @author Ing. E. Migliore
*
* @version 3.0
==============================================================================*/
#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

#define ADC_FREC 200000
#define LM35_FACTOR 10U

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

	PINSEL_CFG_T cfgPin;

	cfgPin.port = PORT_0;
	cfgPin.pin = PIN_23;
	cfgPin.func=PINSEL_FUNC_01;
	cfgPin.mode = PINSEL_TRISTATE;
	cfgPin.openDrain =DISABLE;

	PINSEL_ConfigPin(&cfgPin);

}
uint16_t cuentas_to_celcius (uint16_t reg){
	uint16_t lm35_raw = reg & 0xFFF;

	uint16_t lm35_mv =(lm35_raw* 3300)/4095;

	uint16_t lm35_c= (lm35_mv /LM35_FACTOR);

	return lm35_c;
}
void configTimer(){
	TIM_TIMERCFG_T cfgTm0 ={TIM_US,1};
	TIM_MATCHCFG_T cfgMatchTm0 = {
			.channel = TIM_MATCH_1,
			.intEn = DISABLE,
			.stopEn = DISABLE,
			.resetEn =ENABLE,
			.extOpt = TIM_TOGGLE,
			.matchValue = 999999
	};
	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch (LPC_TIM0,&cfgMatchTm0);

}
void configAdc(){
	//Prendemos el ADC
	ADC_PowerUp();
	//Configuramos los canales
	ADC_PinConfig(ADC_CHANNEL_0);
	//Habilitamos el canal
	ADC_ChannelEnable(ADC_CHANNEL_0);
	//Deshabilitamos el modo Brust
	ADC_BurstDisable();
	//Configuramos el modo de disapro , el flanco y habilitamos las interrupciones
	ADC_StartCmd (ADC_START_ON_MAT01);

	ADC_EdgeStartConfig(ADC_START_ON_RISING);

	ADC_IntEnable(ADC_INT_CH0);
	//Habilitamos la ISR
	NVIC_EnableIRQ(ADC_IRQn);
	//Inicializamos el ADC
	ADC_Init(ADC_FREC);
}
void ADC_IRQHandler(){
	if (ADC_ChannelGetStatus(0,ADC_DATA_DONE)==SET){

		uint16_t temp = cuentas_to_celcius (ADC_ChannelGetData(0));

		if (temp >= 60){
			GPIO_SetPins(PORT_0,(1>22));
		}else{GPIO_ClearPins(PORT_0,(1>22));}
	}
}

int main (){
	configPin();
	configTimer();
	configAdc();
	while(1){
		__WFI();
	}
	return 0;
};
