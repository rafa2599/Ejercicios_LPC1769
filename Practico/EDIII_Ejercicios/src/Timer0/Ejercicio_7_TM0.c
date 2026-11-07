/*Ejercicio 7: Disparo por Hardware para el ADC (MAT 0.1)
Objetivo: Configurar el canal MR1 del Timer 0 para generar un pulso en la línea interna MAT 0.1 a una frecuencia de 10 Hz (tmatch=100ms),
utilizando esta señal como gatillo automático por flanco descendente para iniciar conversiones en el ADC.
Paso a Paso en Registros:
Configuración de Match 1 en Timer 0:
MR1=100.000s-1=99.999.
Configurar MCR con MR1R = 1 (Reset en Match 1).
Configurar EMR con EMC1 = 11 (Toggle en MAT 0.1).
Configuración de Disparo en el ADC (AD0CR):
Bits [26:24] (START): Escribir 100 binario para seleccionar la línea de coincidencia MAT 0.1 como la fuente de disparo por hardware.
Bit 27 (EDGE): Escribir 1 para indicar que el ADC comenzará a medir al detectar el flanco descendente generado por la conmutación de MAT 0.1
 *
 * */

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"

void configPin(){
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
	cfgTm0.prescaleOpt= TIM_US;
	cfgTm0.prescaleValue = 1;

	TIM_MATCHCFG_T cfgMatchTm0;

	cfgMatchTm0.channel=TIM_MATCH_1;
	cfgMatchTm0.intEn = DISABLE;
	cfgMatchTm0.resetEn = ENABLE;
	cfgMatchTm0.stopEn = DISABLE;
	cfgMatchTm0.extOpt = TIM_TOGGLE;
	cfgMatchTm0.matchValue = 9999;
	TIM_Enable(LPC_TIM0);
	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
}
void configAdc(void){
    // 1. Inicializamos el ADC con una frecuencia de reloj de 100 kHz (por ejemplo)
    ADC_Init(100000);

    // 2. Configuramos el pin físico del canal analógico (Ej: Canal 0)
    ADC_PinConfig(ADC_CHANNEL_0);

    // 3. Habilitamos el canal 0 para la conversión
    ADC_ChannelEnable(ADC_CHANNEL_0);

    // 4. Configuramos el ADC para que arranque en el flanco descendente
    ADC_EdgeStartConfig(ADC_START_ON_FALLING);

    // 5. Seleccionamos el evento MAT0.1 como fuente de disparo
    ADC_StartCmd(ADC_START_ON_MAT01);
}

int main (){
	configPin();
	configTimer0();
	configAdc();
	while(1){
		__WFI();
	}
}
