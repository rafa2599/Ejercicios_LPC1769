/*	Se tienen tres bloques de datos de 4KBytes de longitud cada uno en el cual se han guardado tres formas de onda.
    Cada muestra de la onda es un valor de 32 bits que se ha capturado desde el ADC. Las direcciones de inicio de cada bloque
    son representadas por macros del estilo DIRECCION_BLOQUE_N, con N=0,1,2.

	Se pide que, usando DMA y DAC se genere una forma de onda por la salida analógica de la LPC1769.
	La forma de onda cambiará en función de una interrupción externa conectada a la placa de la siguiente manera:

1er interrupción: Forma de onda almacenada en bloque 0, con frecuencia de señal de 60[KHz].
2da interrupción: Forma de onda almacenada en bloque 1 con frecuencia de señal de 120[KHz].
3ra interrupción: Forma de onda almacenada en bloque 0 y bloque 2 (una a continuación de la otra) con frecuencia de señal de 450[KHz].
4ta interrupción: Vuelve a comenzar con la forma de onda del bloque 0.

	En cada caso se debe utilizar el menor consumo de energía posible del DAC.
 *
 */

#include "LPC17xx.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"
#include <stdbool.h>

#define BLOCK1_STRAT 0X2007C000
#define BLOCK1_END   0X2007D000

#define BLOCK2_START 0X2007D000
#define BLOCK2_END   0X2007E000

#define BLOCK3_START 0X2007E000
#define BLOCK3_END   0X2007F000

void configPines(){
	PINSEL_CFG_T cfgPinExt0 = {
			.port = PORT_2,
			.pin = PIN_10,
			.func = PINSEL_FUNC_00,
			.mode = PINSEL_PULLDOWN
	};

	PINSEL_ConfigPin(&cfgPinExt0);
	PINSEL_CFG_T cfgPinExt1 = {
				.port = PORT_2,
				.pin = PIN_11,
				.func = PINSEL_FUNC_00,
				.mode = PINSEL_PULLDOWN
	};

	PINSEL_ConfigPin(&cfgPinExt1);
	PINSEL_CFG_T cfgPinExt2 = {
				.port = PORT_2,
				.pin = PIN_12,
				.func = PINSEL_FUNC_00,
				.mode = PINSEL_PULLDOWN
	};
	PINSEL_ConfigPin(&cfgPinExt2);
	PINSEL_CFG_T cfgPinExt3 = {
				.port = PORT_2,
				.pin = PIN_13,
				.func = PINSEL_FUNC_00,
				.mode = PINSEL_PULLDOWN
	};
	PINSEL_ConfigPin(&cfgPinExt3);

	GPIO_SetDir(PORT_0,(15>10),GPIO_INPUT);
	GPIO_ClearPins(PORT_0,(15>10));
}
void configExt(){

	EXTI_CFG_T cfgExt0={
			.line = EXTI_EINT0,
			.mode =EXTI_EDGE_SENSITIVE,
			.polarity = EXTI_RISING_EDGE
	};

	EXTI_PinConfig(EXTI_EINT0,EXTI_PULLDOWN);
	EXTI_Config(&cfgExt0);
	NVIC_EnableIRQ(EXTI_EINT0);
	EXTI_CFG_T cfgExt1={
			.line = EXTI_EINT1,
			.mode =EXTI_EDGE_SENSITIVE,
			.polarity = EXTI_RISING_EDGE
	};

	EXTI_PinConfig(EXTI_EINT1,EXTI_PULLDOWN);
	EXTI_Config(&cfgExt1);
	NVIC_EnableIRQ(EXTI_EINT1);
	EXTI_CFG_T cfgExt2={
			.line = EXTI_EINT2,
			.mode =EXTI_EDGE_SENSITIVE,
			.polarity = EXTI_RISING_EDGE
	};

	EXTI_PinConfig(EXTI_EINT2,EXTI_PULLDOWN);
	EXTI_Config(&cfgExt2);
	NVIC_EnableIRQ(EXTI_EINT2);
	EXTI_CFG_T cfgExt3={
			.line = EXTI_EINT3,
			.mode =EXTI_EDGE_SENSITIVE,
			.polarity = EXTI_RISING_EDGE
	};

	EXTI_PinConfig(EXTI_EINT3,EXTI_PULLDOWN);
	EXTI_Config(&cfgExt0);
	NVIC_EnableIRQ(EXTI_EINT3);

	EXTI_Init();
}
void EINT0_IRQHandler(){
	if (EXTI_GetFlag(EXTI_EINT0)==SET){
		//Muevo el primer bloque de 4 Kbites
		EXTI_ClearFlag(EXTI_EINT0);
	}
}
void EINT1_IRQHandler(){
	if (EXTI_GetFlag(EXTI_EINT1)==SET){
		//Muevo el 2DO bloque de 4 Kbites
		EXTI_ClearFlag(EXTI_EINT1);
	}
}
void EINT2_IRQHandler(){
	if (EXTI_GetFlag(EXTI_EINT2)==SET){
		//Muevo el 3ER bloque de 4 Kbites
		EXTI_ClearFlag(EXTI_EINT2);
	}
}
void EINT3_IRQHandler(){
	if (EXTI_GetFlag(EXTI_EINT3)==SET){
		//Muevo el primer bloque de 4 Kbites
		EXTI_ClearFlag(EXTI_EINT3);
	}
}
void configDac(){
	DAC_SetBias(0);
}

void configDma(){

}
