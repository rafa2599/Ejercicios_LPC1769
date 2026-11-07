/*-------------------------------------------------------------------------------------------------------------------------------------------------
 *                                                                 MODO BRUST
 *-------------------------------------------------------------------------------------------------------------------------------------------------
 * Programar el microcontrolador LPC1769 para que mediante su ADC digitalice  dos señales analógicas cuyos anchos de bandas son de 10 Khz cada una.
 *  Los canales utilizados deben ser el 2 y el 4 y los datos deben ser guardados en dos regiones de memorias distintas
 *  que permitan contar con los últimos 20 datos de cada canal.
 * Suponer una frecuencia de core cclk de 100 Mhz. El código debe estar debidamente comentado.
 */

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"

#include "LPC17xx.h"
#include "lpc17xx_adc.h"

// 20 muestras * 2 bytes = 40 bytes (0x28)
#define BUFFER1_START 0x2007C000
#define BUFFER1_END   0x2007C028
#define BUFFER2_START 0x2007C028
#define BUFFER2_END   0x2007C050

// Casteo obligatorio para apuntar a memoria física
volatile static uint16_t *bufferA = (volatile uint16_t *)BUFFER1_START;
volatile static uint16_t *bufferB = (volatile uint16_t *)BUFFER2_START;

void configAdc(){
    ADC_PowerUp(); // Encendemos el periférico

    // Configuramos pines físicos y habilitamos los canales lógicos
    ADC_PinConfig(ADC_CHANNEL_2);
    ADC_PinConfig(ADC_CHANNEL_4);
    ADC_ChannelEnable(ADC_CHANNEL_2);
    ADC_ChannelEnable(ADC_CHANNEL_4);

    // Tasa de 40 kHz (20 kHz por canal para cumplir Nyquist)
    ADC_Init(40000);

    // Habilitamos escaneo por hardware (BURST)
    ADC_BurstEnable();

    // Habilitamos las interrupciones
    ADC_IntEnable(ADC_INT_CH2);
    ADC_IntEnable(ADC_INT_CH4);
    NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_IRQHandler(){
    // El bit 2 del AD0INTEN dispara interrupción del canal 2
    if (ADC_ChannelGetStatus(ADC_CHANNEL_2, ADC_DATA_DONE) == SET){
        // Leemos el dato del registro ADDR2
        *bufferA = ADC_ChannelGetData(ADC_CHANNEL_2);
        bufferA++; // Avanzamos la dirección de memoria

        // Lógica de Buffer Circular: si llegamos al final, volvemos al inicio
        if (bufferA >= (volatile uint16_t *)BUFFER1_END){
            bufferA = (volatile uint16_t *)BUFFER1_START;
        }
    }

    // El bit 4 del AD0INTEN dispara interrupción del canal 4
    if (ADC_ChannelGetStatus(ADC_CHANNEL_4, ADC_DATA_DONE) == SET){
        // Leemos el dato del registro ADDR4
        *bufferB = ADC_ChannelGetData(ADC_CHANNEL_4);
        bufferB++;

        if (bufferB >= (volatile uint16_t *)BUFFER2_END){
            bufferB = (volatile uint16_t *)BUFFER2_START;
        }
    }
}

int main(){
    configAdc();
    while(1){
        __WFI(); // Dormimos la CPU para ahorrar energía
    }
    return 0;
}
