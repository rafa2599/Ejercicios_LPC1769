/*------------------------------------------------------------------------------------------------------------------------------------------------
 *                                                                 MODO MANUAL
 *-------------------------------------------------------------------------------------------------------------------------------------------------
 * Programar el microcontrolador LPC1769 para que mediante su ADC digitalice  dos señales analógicas cuyos anchos de bandas son de 10 Khz cada una.
 *  Los canales utilizados deben ser el 2 y el 4 y los datos deben ser guardados en dos regiones de memorias distintas
 *  que permitan contar con los últimos 20 datos de cada canal.
 * Suponer una frecuencia de core cclk de 100 Mhz. El código debe estar debidamente comentado.
 */


#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"


// 20 muestras * 2 bytes = 40 bytes (0x28)
#define BUFFER1_START 0x2007C000
#define BUFFER1_END   0x2007C028
#define BUFFER2_START 0x2007C028
#define BUFFER2_END   0x2007C050

// Casteo obligatorio para apuntar a memoria física
volatile static uint16_t *bufferA = (volatile uint16_t *)BUFFER1_START;
volatile static uint16_t *bufferB = (volatile uint16_t *)BUFFER2_START;

void configTimer(){
	TIM_TIMERCFG_T cfgTm0 = {TIM_US,1};
	TIM_MATCHCFG_T cfgMatchTm0 = {
			.channel = TIM_MATCH_1,
			.intEn = ENABLE,
			.resetEn =ENABLE,
			.matchValue = 24 //
	};
	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
	TIM_ClearIntPending(LPC_TIM0,TIM_MR1_INT);
	TIM_Enable(LPC_TIM0);
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void configAdc(){

	void configAdc(){
	    ADC_PowerUp(); // Encendemos el periférico

	    // Configuramos pines físicos y habilitamos los canales lógicos
	    ADC_PinConfig(ADC_CHANNEL_2);
	    ADC_PinConfig(ADC_CHANNEL_4);
	    ADC_ChannelEnable(ADC_CHANNEL_2);
	    //ADC_ChannelEnable(ADC_CHANNEL_4);
	    ADC_StartCmd (ADC_START_ON_MAT01);
	    // Tasa de 40 kHz (20 kHz por canal)
	    ADC_Init(40000);

}
void TIMER0_IRQHandler(){


	while (ADC_ChannelGetStatus(2,ADC_DATA_DONE)== RESET){
        // Leemos el dato del registro ADDR2
        *bufferA = ADC_ChannelGetData(ADC_CHANNEL_2);
        bufferA++; // Avanzamos la dirección de memoria

        // Lógica de Buffer Circular: si llegamos al final, volvemos al inicio
        if (bufferA >= (volatile uint16_t *)BUFFER1_END){
            bufferA = (volatile uint16_t *)BUFFER1_START;
        }


	}
	ADC_ChannelDisable (2);
	ADC_ChannelEnable(4);
	while (ADC_ChannelGetStatus(4,ADC_DATA_DONE)== RESET){

        // Leemos el dato del registro ADDR2
        *bufferB = ADC_ChannelGetData(ADC_CHANNEL_4);
        bufferB++; // Avanzamos la dirección de memoria

        // Lógica de Buffer Circular: si llegamos al final, volvemos al inicio
        if (bufferB >= (volatile uint16_t *)BUFFER2_END){
            bufferB = (volatile uint16_t *)BUFFER2_START;
        }

	}
	TIM_ClearIntPending(LPC_TIM0,TIM_MR1_INT);

}
}
int main (void){
	configTimer();
	configAdc();
	while(1){
		__WFI();
	}

	return 0;
};


