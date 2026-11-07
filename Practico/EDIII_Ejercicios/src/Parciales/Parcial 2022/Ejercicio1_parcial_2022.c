/*Utilizando el timer0, un dac, interrupciones y el driver del LPC1769 , escribir un código que permita generar una señal triangular periódica
 * simétrica, que tenga el mínimo periodo posible, la máxima excursión de voltaje pico a pico posible y el mínimo incremento de señal posible
 *  por el dac. Suponer una frecuencia de core cclk de 100 Mhz.
 *  El código debe estar debidamente comentado
 * */
#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_dac.h"
#include <stdbool.h>
static const uint16_t max_value = 1023;
void configTimer(){

	TIM_TIMERCFG_T cfgTm0 = {TIM_US,1};
	TIM_MATCHCFG_T cfgMatchTm0 ={
			.channel =TIM_MATCH_1,
			.intEn = ENABLE,
			.stopEn = DISABLE,
			.resetEn =ENABLE,
			.extOpt= TIM_NOTHING,
			.matchValue = 0
	};
	TIM_InitTimer(LPC_TIM0,&cfgTm0);
	TIM_ConfigMatch(LPC_TIM0,&cfgMatchTm0);
	TIM_ClearIntPending(LPC_TIM0,TIM_MR1_INT);
	TIM_Enable(LPC_TIM0);
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void configDac(){
	DAC_Init();
	DAC_SetBias(0); //700 mA

}

void TIMER0_IRQHandler() {
    static uint16_t counter = 0;
    static bool up = true;

    // 1. Escribimos el valor actual al hardware
    DAC_UpdateValue(counter);

    // 2. Calculamos el próximo estado
    if (up) {
        counter++;
        if (counter >= max_value) {
            up = false;
        }
    } else {
        counter--;
        if (counter == 0) {
            up = true;
        }
    }

    // 3. Limpiamos la bandera del Timer
    TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);
}




int main (){
	configTimer();
	configDac();
	while(1){
		__WFI();
	}
}
