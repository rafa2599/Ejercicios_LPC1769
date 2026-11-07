#include "LPC17xx.h"

/* DEFINICIÓN DE CONSTANTES */
#define DSPL_MAX_DIGITS  10
#define DELAY_TIME       10000000
#define DATA_MASK_DSPL   0xFF
#define CTRL_DSPL        (1 << 10)

/* DEFINICIÓN DE VARIABLES GLOBALES */
const uint32_t lutDSPL_CC[5] =
{
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x67  // 9
};

/* PROTOTIPOS DE FUNCIONES */
void cfgGPIO(void);
void countDigitDSPL(void);
void printDigitDSPL(uint8_t);
void delay(uint32_t);

/* FUNCIÓN PRINCIPAL */
int main(void)
{
    cfgGPIO();
    while(1)
    {
        countDigitDSPL();
    }
    return 0;
}

/* DEFINICIONES DE FUNCIONES */
void cfgGPIO(void)
{
    LPC_GPIO2->FIODIR |= (DATA_MASK_DSPL | CTRL_DSPL);
    LPC_GPIO2->FIOSET |= CTRL_DSPL;
    return;
}

void countDigitDSPL(void)
{
    uint8_t i;
    for(i = 0; i < DSPL_MAX_DIGITS; i++)
    {
        printDigitDSPL(lutDSPL_CC[i]);
        delay(DELAY_TIME);
    }
    return;
}

void printDigitDSPL(uint8_t digit)
{
    LPC_GPIO2->FIOCLR = DATA_MASK_DSPL;
    LPC_GPIO2->FIOSET = digit;
    return;
}

void delay(uint32_t delayTime)
{
    for(volatile uint32_t i = 0; i < delayTime; i++)
    {
        __NOP();
    }
    return;
}
