/*Escribir un programa en codigo C, de forma tal que varias señales analogicas se guarden en un buffer de memoria y que luego de un minuto
 *  se pare el ADC y se pueda leer el buffer para sacar nuevamente esas señales.
 * Para ello utilizar ADC,DAC,TIMER Y GPDMA. De esta forma el ADC debe convertir  y guardar las señales durante un 1 minuto y luego durante otro minuto
 * el DAC debe leer el buffer y sacar dichas señales (P0.26) .
 * @details:
 *  El ADC debe trabajar en modo BURST guardando datos en un buffer por 1 minutos
 *  El buffer debe ser circular
 *  Se debe usar GPDMA
 *  Se debe usar Timer
 *
 */

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"
#include <stdbool.h>

#define FREC_ADC 200000U
#define TEMP_DAC 125
#define BUFFER_ADC_START 0X2007C000
#define BUFFER_DAC_START 0X2007C000



volatile static uint32_t* ptrAdc = (uint32_t*)BUFFER_ADC_START;
volatile static uint32_t* ptrDac = (uint32_t*) BUFFER_DAC_START;

volatile static bool writing = true;




volatile static GPDMA_LLI_T ADCbufferLLI = {
		.srcAddr = (uint32_t)&LPC_ADC->ADGDR,
		.dstAddr = ptrAdc,
		.nextLLI = (uint32_t)&ADCbufferLLI,
		.control = (1024 & 0xFFF )| (0<<12) | (0 << 15) | (1<<18) | (1<<21) | (0<26) | (1<<27) | (0<<31)

};
volatile static GPDMA_LLI_T DACbufferLLI = {
		.srcAddr = ptrDac,
		.dstAddr = (uint32_t)&LPC_DAC->DACR,
		.nextLLI = (uint32_t)&DACbufferLLI,
		.control = (1024 & 0xFFF ) | (0<<12) | (0 << 15) | (1<<18) | (1<<21) | (1<26) | (0<<27) | (0<<31)
};


void configPines();
void configAdc();
void configTimer();
void configDmaADC();
void configDmaDAC();
int main(){
	configPines();
	configAdc();
	configTimer();
	configDmaADC();
	configDmaDAC();
	while(1){
		__WFI();
	}
};


void configPines(){
	PINSEL_CFG_T cfgPinAdc = {
			.port =PORT_0,
			.pin = PIN_23,
			.func = PINSEL_FUNC_01,
			.mode = PINSEL_TRISTATE,
			.openDrain =DISABLE
	};
	PINSEL_ConfigPin(&cfgPinAdc);
}
void configAdc(){
	ADC_PowerUp();
	ADC_BurstEnable();
	//ADC_StartCmd(ADC_START_NOW);
	ADC_ChannelEnable(ADC_CHANNEL_0);
	ADC_Init(FREC_ADC);

}

void configDac(){
	DAC_CONVERTER_CFG_T cfgDac = {ENABLE,ENABLE,ENABLE};

	DAC_Init();
	DAC_ConfigDAConverterControl(&cfgDac);
	DAC_SetDMATimeOut(TEMP_DAC);
}
void configDmaADC(){
	GPDMA_Endpoint_T cfgSrc = {GPDMA_HALFWORD,GPDMA_BSIZE_1,DISABLE};
	GPDMA_Endpoint_T cfgDst = {GPDMA_HALFWORD,GPDMA_BSIZE_1,ENABLE};
	GPDMA_Channel_CFG_T cfgDma ={
			.channelNum =GPDMA_CH_0,
			.transferSize = 1024,
			.type = GPDMA_P2M,
			.srcMemAddr = (uint32_t)&LPC_ADC->ADGDR,
			.dstMemAddr = prtAdc,
			.srcConn =GPDMA_ADC,
			.src = cfgSrc ,
			.dst = cfgDst,
			.linkedList = (uint32_t)&ADCbufferLLI


	};
	GPDMA_Init();
	GPDMA_SetupChannel(&cfgDma);

}
void configDmaDAC(){
	GPDMA_Endpoint_T cfgSrc1 = {GPDMA_HALFWORD,GPDMA_BSIZE_1,ENABLE};
	GPDMA_Endpoint_T cfgDst1 = {GPDMA_HALFWORD,GPDMA_BSIZE_1,DISABLE};
	GPDMA_Channel_CFG_T cfgDac ={
			.channelNum = GPDMA_CH_1,
			.transferSize = 1024,
			.type = GPDMA_M2P,
			.srcMemAddr = ptrDac,
			.dstMemAddr = (uint32_t)&LPC_DAC->DACR,
			.dstConn=GPDMA_DAC,
			.src = cfgSrc1,
			.dst =cfgDst1,
			.linkedList = (uint32_t)&DACbufferLLI
	};
	GPDMA_SetupChannel(&cfgDac);
}

void configTimer(){
	TIM_TIMERCFG_T cfgTm0 = {TIM_US,1};
	TIM_MATCHCFG_T cfgMatchTm0={
			.channel = TIM_MATCH_0,
			.intEn = ENABLE,
			.stopEn= DISABLE,
			.resetEn = ENABLE,
			.extOpt =TIM_NOTHING,
			.matchValue = 59999999

	};
	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
	TIM_Enable(LPC_TIM0);
	NVIC_EnableIRQ(TIMER0_IRQn);

}
void TIMER0_IRQHandler(){
	//CASO LEYENDEO :El ADC ya convirtió y escribió en el buffer durante 1 min
	if (TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT)==SET && !writing){
		//	1- Desactivar el ADC
		ADC_DeInit();
		// 2- Desactivar  el canal 0 del GPDAM que conecta ADC con el buffer
		GPDMA_ChannelStop(GPDMA_CH_0);
		// 3- Activar el canal 1 del GPDMA que conecta el DAC con el buffer
		GPDMA_ChannelStart (GPDMA_CH_1);
		// 4- Activar el DAC
		configDac();
		// 5 - Cambiar la condicion writing
		writing = true;
		// 6- Limpiar la flag de interrupcion
		TIM_ClearIntPending (LPC_TIM0,TIM_MR0_INT);
	}
	//CASO ESCRIBIENDO: El DAC ya leyó el buffer y convirtió durante 1 min
	else if (TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT)==SET && writing){
		// 1- Desactivo el DAC
		LPC_SC ->PCONP &=~ (1 <<22);
		// 2-Desactivo el Canal 1 del GPDMA
		GPDMA_ChannelStop(GPDMA_CH_1);
		// 3- Activo el canal 0 del GPDMA
		GPDMA_ChannelStart(GPDMA_CH_0);
		// 4- Activo el ADC
		configAdc();
		// 5- Cambio la condicion de writing
		writing = false;
		//6 -Limpio la flag del timer
		TIM_ClearIntPending (LPC_TIM0,TIM_MR0_INT);
	}
}

