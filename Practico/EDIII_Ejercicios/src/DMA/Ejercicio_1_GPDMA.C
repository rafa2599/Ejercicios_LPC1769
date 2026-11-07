/*Hacer un programa en codigo C para una placa LPC1769 que mediante la utilizacion del DAC y GPDMA permita
 * mostrar la salida del DAC (P0.26 ) las senales guardadas  en memoria
 * */

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"

#define DAC_FREC_MUESTREO 125
#define BUFFER_START 0X2007C00
void configPin(){
	PINSEL_CFG_T cfgDac ={
			.port =PORT_0,
			.pin=PIN_26,
			.func =PINSEL_FUNC_01,
			.mode = PINSEL_TRISTATE,
			.openDrain = DISABLE
	};
	PINSEL_ConfigPin(&cfgDac);
}
void configDac (){
	DAC_Init();
	DAC_CONVERTER_CFG_T cfgDac = {
			.doubleBuffer =ENABLE,
			.dmaCounter = ENABLE,
			.dmaRequest =ENABLE
	};
	DAC_ConfigDAConverterControl(&cfgDac);
	DAC_SetDMATimeOut(DAC_FREC_MUESTREO);

}
void configDMA(){
	GPDMA_Endpoint_T cfgSrc = {
			.width = GPDMA_HALFWORD,
			.burst = GPDMA_BSIZE_1,
			.increment = ENABLE
	};
	GPDMA_Endpoint_T  cfgDst = {GPDMA_HALFWORD,GPDMA_BSIZE_1,DISABLE};
	GPDMA_Channel_CFG_T cfgDma={
			.channelNum=0,
			.transferSize = 256,
			.type =GPDMA_M2P,
			.srcMemAddr =  BUFFER_START,
			.dstMemAddr = (unint32_t)&LPC_DAC->DACR,
			.dstConn =GPDMA_DAC,
			.src =cfgSrc,
			.dst = cfgDst,
			.intTC = DISABLE,
			.intErr =ENABLE,
			.linkedList = 0
	};
	GPDMA_Init();
	GPDMA_SetupChannel (&cfgDma);
	GPDMA_ChannelStart(GPDMA_CH_0);
	NVIC_EnableIRQ(DMA_IRQn);

}
void DMA_IRQHandler(){ //

	if (GPDMA_IntGetStatus(GPDMA_INTERR,GPDMA_CH_0)==SET){
		GPDMA_ChannelStop(GPDMA_CH_0);
		GPDMA_ClearIntPending(GPDMA_INTERR,GPDMA_CH_0);
	}
}
int main (){
	configPin();
	configDac();
	configDma();
	while(1){
		__WFI();
	}
	return 0;

};
