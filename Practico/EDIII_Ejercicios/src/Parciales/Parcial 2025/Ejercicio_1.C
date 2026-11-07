/*Mediante la utilización del microcontrolador LPC1769, implemente el software necesario  para cumplir con los siguientes requerimientos:
*1- Usar el hardware GPDMA para transferir datos en palabras de 2 bytes desde la primera mitad de la SRAM del banco 1
* hacia la segunda mitad de la SRAM del banco 0.
* 2- Una vez finalizada la transferencia, mediante la interrupción por cuenta terminal del GPDMA,
* se debe encender un LED indicador conectado a P0.22
 * */
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_gpdma.h"


#define BANK0_START_ADDRES 0x2007E000  // Segunda mitad del banco 0
#define BANK1_START_ADDRES 0x20080000 //  Primera mitad del banco 1


void configPin(){
	PINSEL_CFG_T cfgLed = {
			.port = PORT_0,
			.pin = PIN_22,
			.func = PINSEL_FUNC_00,
			.mode = PINSEL_PULLDOWN,
			.openDrain = DISABLE
	};
	PINSEL_ConfigPin(&cfgLed);
	GPIO_SetDir(PORT_0,1<<22,GPIO_OUTPUT);
	GPIO_ClearPins(PORT_0,1<<22);
}

void configDma (){
	GPDMA_Endpoint_T cfgSrc = {GPDMA_HALFWORD,GPDMA_BSIZE_1,ENABLE};
	GPDMA_Endpoint_T cfgDst = {GPDMA_HALFWORD,GPDMA_BSIZE_1,ENABLE};
	GPDMA_Channel_CFG_T cfgDma ={
			.channelNum= GPDMA_CH_0,
			.transferSize = 4095,
			.type =GPDMA_M2M,
			.srcMemAddr = BANK1_START_ADDRES,
			.dstMemAddr = BANK0_START_ADDRES,
			.srcConn = 0,
			.dstConn = 0,
			.src = cfgSrc,
			.dst =cfgDst,
			.intTC =ENABLE,
			.intErr = ENABLE,
	};
	GPDMA_Init();

	GPDMA_SetupChannel(&cfgDma);
	GPDMA_ChannelStart(GPDMA_CH_0);
	NVIC_EnableIRQ(DMA_IRQn);

}

void DMA_IRQHandler(){
	if (GPDMA_IntGetStatus(GPDMA_INTTC,GPDMA_CH_0)==SET){
		GPIO_SetPins(PORT_0,1<<22);
		GPDMA_ClearIntPending(GPDMA_INTTC,GPDMA_CH_0);
	}
	else if (GPDMA_IntGetStatus(GPDMA_INTERR,GPDMA_CH_0)==SET){
		GPDMA_ChannelStop(GPDMA_CH_0);
	}
}
int main(){};
