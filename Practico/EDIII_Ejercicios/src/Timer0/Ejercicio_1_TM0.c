/*Ejercicio 1: Configuración de la Base de Tiempo Básica
Objetivo: Comprender la jerarquía de relojes del sistema y ajustar el preescalador para lograr una resolución exacta de 1 microsegundo (1 µs) por tick en el contador principal TC.
Contexto y Datos de Partida:
Frecuencia del reloj principal de CPU: CCLK=100MHz.
Reloj del periférico por defecto: PCLK=CCLK4=25MHz4.
Queremos que el contador de tiempo TC se incrementa cada 1s */
#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"

void confTimer0 (){
	TIM_TIMERCFG_T confTimerMode;
	confTimerMode.prescaleOpt = TIM_US ;
	confTimerMode.prescaleValue = 1 ;
	TIM_InitTimer(LPC_TIM0,&confTimerMode);
	TIM_Enable (LPC_TIM0);
}


int main(void){

};
