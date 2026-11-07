/*
 * Ejercicio 1: Voltímetro Básico por Software (Nivel Inicial)
Objetivo: Aprender a disparar conversiones del ADC mediante software sincronizado por interrupción de temporizador.
Planteo:
Configurar el Timer 0 para generar una interrupción por software cada 100 ms (10 Hz).
En la rutina de servicio de interrupción del Timer (TIMER0_IRQHandler), iniciar manualmente una conversión en el canal AD0.0
utilizando disparo por software (ADC_START_NOW).
En la rutina ADC_IRQHandler, leer las cuentas de 12 bits, calcular la tensión de entrada (0a3.3V)
y encender un LED en P0.22 únicamente si la tensión supera los 2.0 V.
Pasos de Razonamiento:
¿Cómo configurar el PR y el MR0 del Timer 0 para obtener exactamente 100 ms con PCLK=25MHz?
Recordá habilitar la interrupción por coincidencia del Timer (IntOnMatch = ENABLE)
e incluir la limpieza del flag de interrupción en TIMER0_IRQHandler.
 *
 */
#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"

#define rate_samples 10000


void configPin(){
	PINSEL_CFG_T cfgPin;

	cfgPin.port = PORT_0;
	cfgPin.pin = PIN_22;
	cfgPin.func=PINSEL_FUNC_00;
	cfgPin.mode = PINSEL_PULLDOWN;
	cfgPin.openDrain =DISABLE;

	PINSEL_ConfigPin(&cfgPin);
	GPIO_SetDir(PORT_0,(1<<22),GPIO_OUTPUT);
	GPIO_ClearPins(PORT_0,(1<<22));


}

void configTimer0(){

	TIM_TIMERCFG_T cfgTm0;
	cfgTm0.prescaleOpt = TIM_US;
	cfgTm0.prescaleValue = 1 ;

	TIM_MATCHCFG_T cfgMatchTm0;

	cfgMatchTm0.channel = TIM_MATCH_1;
	cfgMatchTm0.intEn = ENABLE;
	cfgMatchTm0.stopEn =DISABLE;
	cfgMatchTm0.resetEn = ENABLE;
	cfgMatchTm0.extOpt = TIM_NOTHING;
	cfgMatchTm0.matchValue = 99999;

	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
	TIM_ClearIntPending(LPC_TIM0,TIM_MR1_INT);
	TIM_Enable(LPC_TIM0);
	NVIC_EnableIRQ(TIMER0_IRQn);
}
void configAdc(){
	ADC_PowerUp();
	ADC_PinConfig(ADC_CHANNEL_0);
	ADC_EdgeStartConfig(ADC_START_ON_RISING);
	ADC_ChannelEnable(ADC_CHANNEL_0);
	ADC_IntEnable(ADC_INT_CH0);
	ADC_Init(rate_samples);
	NVIC_EnableIRQ(ADC_IRQn);

}
void TIMER0_IRQHandler(){

	if (TIM_GetIntStatus(LPC_TIM0,TIM_MR1_INT)==SET){
		ADC_StartCmd(ADC_START_NOW);
		TIM_ClearIntPending(LPC_TIM0,TIM_MR1_INT);
	}
}

void ADC_IRQHandler(){
	if (ADC_GlobalGetStatus(ADC_DATA_DONE) == SET){

	        // 1. Leemos el valor del ADC (entero de 0 a 4095)
	        uint16_t resultado_adc = ADC_ChannelGetData(ADC_CHANNEL_0);

	        // 2. Comparamos contra nuestro umbral fijo (2482 cuentas == 2.0V)
	        if (resultado_adc > 2482) {
	            // Si la tensión es mayor a 2.0V, encendemos el LED en P0.22
	            // Usamos la máscara de bits para el pin 22
	            GPIO_SetPins(PORT_0, (1<<22));
	        } else {
	            // Si es menor o igual, apagamos el LED
	            GPIO_ClearPins(PORT_0, (1<<22));
	        }
	    }
}

int main (){
	configPin();
	configTimer0();
	configAdc();

	while(1){
		__WFI();
	}
	return 0 ;
}
