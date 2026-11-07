/*Por un pin del ADC del microcontrolador LPC1769 ingresa una tensión de rango dinámico 0 a 3,3[v] proveniente de un sensor de temperatura.
Debido a la baja tasa de variación de la señal, se pide tomar una muestra cada 30[s].
Pasados los 2[min] se debe promediar las últimas 4 muestras y en función de este valor, tomar una decisión
sobre una salida digital de la placa:

 * Si el valor es <1 [V] colocar la salida en 0 (0[V]).
 * Si el valor es >= 1[V] y <=2[V] modular una señal PWM con un Ciclo de trabajo que va desde el 50% hasta el 90%  proporcional al valor
de tensión, con un periodo de 20[KHz].
 * Si el valor es > 2[V] colocar la salida en 1 (3,3[V]).

 */
#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include <stdbool.h>

volatile uint16_t buffer[4];
volatile uint32_t lm35_mv = 0;
volatile uint32_t muestras_prom = 0;

void configPin(void) {
    PINSEL_CFG_T cfgPin;
    cfgPin.port = PORT_0;
    cfgPin.pin = PIN_22;
    cfgPin.func = PINSEL_FUNC_00; // Función GPIO
    cfgPin.mode = PINSEL_PULLDOWN;
    cfgPin.openDrain = DISABLE;

    PINSEL_ConfigPin(&cfgPin);
    GPIO_SetDir(PORT_0, (1<<22), 1); // 1 = OUTPUT
    GPIO_ClearPins(PORT_0, (1<<22));

    PINSEL_CFG_T cfgPinAdc = {
    		.port = PORT_0,
			.pin= PIN_23,
			.func = PINSEL_FUNC_01,
			.mode = PINSEL_TRISTATE,
			.openDrain = DISABLE
    };
    PINSEL_ConfigPin(&cfgPinAdc);
}

void configTimer0(void) {
    // Timer 0: Dedicado exclusivamente a contar 30s y disparar el ADC
    TIM_TIMERCFG_T cfgTm0 = {TIM_US, 1};

    // Usamos MATCH_1 porque el ADC se dispara con ADC_START_ON_MAT01
    TIM_MATCHCFG_T cfgMatchTm0 = {
        .channel = TIM_MATCH_1,
        .intEn = DISABLE,      // No necesitamos interrumpir a la CPU
        .resetEn = ENABLE,     // Resetea a 0 al llegar a 30s
        .stopEn = DISABLE,
        .extOpt = TIM_TOGGLE,
        .matchValue = 29999999 // 30 segundos en microsegundos
    };

    TIM_InitTimer(LPC_TIM0, &cfgTm0);
    TIM_ConfigMatch(LPC_TIM0, &cfgMatchTm0);
    TIM_Enable(LPC_TIM0);
}

void configTimer1(void) {
    // Timer 1: Generación de PWM por software (Periodo 50us -> 20kHz)
    TIM_TIMERCFG_T cfgTm1 = {TIM_US, 1};
    TIM_InitTimer(LPC_TIM1, &cfgTm1);

    // MR0: Controla el periodo total (50 us)
    TIM_MATCHCFG_T cfgMatchTm0 = {
        .channel = TIM_MATCH_0,
        .intEn = ENABLE,       // Interrumpe para encender el pin
        .resetEn = ENABLE,     // Resetea el timer para el nuevo ciclo
        .stopEn = DISABLE,
        .extOpt = TIM_NOTHING,
        .matchValue = 49       // 0 a 49 = 50 cuentas = 50 us
    };
    TIM_ConfigMatch(LPC_TIM1, &cfgMatchTm0);

    // MR1: Controla el ciclo de trabajo (arranca en 0)
    TIM_MATCHCFG_T cfgMatchTm1 = {
        .channel = TIM_MATCH_1,
        .intEn = ENABLE,       // Interrumpe para apagar el pin
        .resetEn = DISABLE,    // NO resetea, deja que llegue hasta MR0
        .stopEn = DISABLE,
        .extOpt = TIM_NOTHING,
        .matchValue = 0        // Se actualiza luego en la función pwm()
    };
    TIM_ConfigMatch(LPC_TIM1, &cfgMatchTm1);

    TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
    TIM_ClearIntPending(LPC_TIM1, TIM_MR1_INT);
    NVIC_EnableIRQ(TIMER1_IRQn);
}

void configAdc(void) {
    ADC_PowerUp();
    ADC_PinConfig(ADC_CHANNEL_2);
    ADC_ChannelEnable(ADC_CHANNEL_2);

    // El hardware del ADC espera el flanco del MAT0.1 para iniciar conversión
    ADC_BurstDisable();
    ADC_StartCmd(ADC_START_ON_MAT01);
    ADC_Init(40000);

    NVIC_EnableIRQ(ADC_IRQn);
}

uint32_t promedioMuestras(void) {
    uint32_t val_prom = 0; // Usar 32 bits para sumar sin desborde
    for (uint8_t i = 0; i < 4; i++) {
        val_prom += buffer[i];
    }
    return (val_prom / 4);
}

void pwm(uint32_t mv) {
    // Ecuación de la recta con enteros puros: DutyCycle = (40*mV / 1000) + 10
    uint32_t duty_cycle = ((40 * mv) / 1000) + 10;

    // Escalamiento del porcentaje a las cuentas del Timer (0 a 49)
    uint32_t matchVal = (duty_cycle * 49) / 100;

    // Inyectamos el nuevo ciclo de trabajo al registro Match 1
    TIM_UpdateMatchValue(LPC_TIM1, TIM_MATCH_1, matchVal);
    TIM_Enable(LPC_TIM1);
}

void celcius_to_volts(uint16_t val) {
    uint16_t lm35_raw = val & 0xFFF;
    lm35_mv = ((uint32_t)lm35_raw * 3300) / 4095;

    if (lm35_mv < 1000) {
        TIM_Disable(LPC_TIM1); // Apagamos el motor PWM
        GPIO_ClearPins(PORT_0, (1<<22)); // Forzamos 0V
    }
    else if (lm35_mv > 2000) {
        TIM_Disable(LPC_TIM1); // Apagamos el motor PWM
        GPIO_SetPins(PORT_0, (1<<22)); // Forzamos 3.3V
    }
    else {
        pwm(lm35_mv); // Entramos en rango de modulación
    }
}

void ADC_IRQHandler(void) {
    static uint8_t num_muestras = 0;

    if (ADC_ChannelGetStatus(2, ADC_DATA_DONE) == SET) {
        buffer[num_muestras] = ADC_ChannelGetData(2);
        num_muestras++;

        // Cuando llegamos a 4 muestras (2 minutos), procesamos
        if (num_muestras >= 4) {
            muestras_prom = promedioMuestras();
            celcius_to_volts((uint16_t)muestras_prom);
            num_muestras = 0;
        }
    }
}

void TIMER1_IRQHandler(void) {
    // MR0: Inicia el periodo total de 50us -> Encendemos la señal
    if (TIM_GetIntStatus(LPC_TIM1, TIM_MR0_INT) == SET) {
        TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
        GPIO_SetPins(PORT_0, (1<<22));
    }

    // MR1: Finaliza el ciclo de trabajo activo -> Apagamos la señal
    if (TIM_GetIntStatus(LPC_TIM1, TIM_MR1_INT) == SET) {
        TIM_ClearIntPending(LPC_TIM1, TIM_MR1_INT);
        GPIO_ClearPins(PORT_0, (1<<22));
    }
}

int main(void) {
    configPin();
    configTimer0();
    configTimer1();
    configAdc();

    while(1) {
        __WFI(); // Wait For Interrupt: dormimos el core para ahorrar energía
    }
    return 0;
}



