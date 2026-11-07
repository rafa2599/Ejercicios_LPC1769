# Electrónica Digital III – Ejercicios sobre LPC1769

Repositorio con el material teórico y la resolución de los ejercicios prácticos desarrollados durante el cursado de **Electrónica Digital III (EDIII)**. Todo el código está escrito en **C** para el microcontrolador **NXP LPC1769** (ARM Cortex-M3) montado en la placa **LPCXpresso LPC1769 (Rev. B)**.

Los ejercicios recorren, de forma progresiva, los periféricos principales del microcontrolador: puertos GPIO, interrupciones, SysTick, temporizadores, ADC, DAC y DMA, e incluyen además la resolución de parciales de años anteriores.

---

## Tabla de contenidos

1. [Hardware y entorno de desarrollo](#1-hardware-y-entorno-de-desarrollo)
2. [Estructura del repositorio](#2-estructura-del-repositorio)
3. [Parte práctica: ejercicios](#3-parte-práctica-ejercicios)
   - [3.1 Puertos GPIO y PINSEL](#31-puertos-gpio-y-pinsel)
   - [3.2 GPIO: polling e interrupciones](#32-gpio-polling-e-interrupciones)
   - [3.3 Interrupciones externas](#33-interrupciones-externas)
   - [3.4 SysTick](#34-systick)
   - [3.5 Timer 0](#35-timer-0)
   - [3.6 ADC](#36-adc)
   - [3.7 DMA (GPDMA)](#37-dma-gpdma)
   - [3.8 Parciales](#38-parciales)
4. [Librería CMSIS y drivers](#4-librería-cmsis-y-drivers)
5. [Parte teórica](#5-parte-teórica)
6. [Cómo compilar y ejecutar](#6-cómo-compilar-y-ejecutar)
7. [Conceptos y técnicas utilizadas](#7-conceptos-y-técnicas-utilizadas)
8. [Estado del repositorio y mejoras pendientes](#8-estado-del-repositorio-y-mejoras-pendientes)

---

## 1. Hardware y entorno de desarrollo

| Elemento | Detalle |
|---|---|
| Microcontrolador | NXP **LPC1769** (ARM Cortex-M3) |
| Placa | LPCXpresso LPC1769 Rev. B |
| Frecuencia de CPU (CCLK) | 100 MHz (80 MHz en el Parcial 1 A) |
| Reloj de periféricos (PCLK) | CCLK/4 = 25 MHz (valor por defecto) |
| IDE | MCUXpresso IDE (Eclipse CDT, toolchain *NXP MCU Tools*) |
| Compilador | `arm-none-eabi-gcc` |
| Librerías | CMSIS v2.00 para LPC17xx y drivers de NXP (`lpc17xx_*.h`) |
| Binario generado | `.axf` |

Los ejercicios usan dos estilos de programación:

- **Acceso directo a registros** (`LPC_PINCON->PINSEL0`, `LPC_GPIO0->FIOSET`, etc.), usado en las primeras unidades para entender el hardware a bajo nivel.
- **Drivers de NXP** (`PINSEL_ConfigPin`, `TIM_ConfigMatch`, `ADC_Init`, `GPDMA_SetupChannel`, …), usado desde Timer0 en adelante para trabajar a más alto nivel.

---

## 2. Estructura del repositorio

```
Ejercicios_LPC1769/
├── Practico/
│   ├── CMSISv2p00_LPC17xx/        # Librería CMSIS + drivers NXP (proyecto de librería)
│   │   ├── Drivers/
│   │   │   ├── inc/               # Headers de los drivers (gpio, timer, adc, dac, gpdma, ...)
│   │   │   ├── src/               # Implementación de los drivers
│   │   │   └── tests/             # Tests de los drivers
│   │   └── src/                   # core_cm3.c, system_LPC17xx.c
│   │
│   └── EDIII_Ejercicios/          # Proyecto principal con todos los ejercicios
│       └── src/
│           ├── Puertos GPIO/
│           ├── Interrupciones GPIO/
│           ├── Interrupciones Externas/
│           ├── SysTick/
│           ├── Timer0/
│           ├── ADC/
│           ├── DMA/
│           ├── Parciales/
│           ├── EDIII_Ejercicios.c         # main de plantilla (Hello World)
│           ├── cr_startup_lpc175x_6x.c    # Código de arranque (vector de interrupciones)
│           └── crp.c                      # Code Read Protection
│
├── Teorico/
│   ├── Clases/                    # Apuntes de las clases (PDF)
│   ├── Documentación_LPC1769/     # Datasheet, manual de usuario, esquemático, tablas
│   └── Ejercicios/                # Enunciados de los ejercicios propuestos (PDF)
│
├── .gitignore
└── README.md
```

> **Nota:** cada archivo `.c` dentro de `src/` contiene su propio `main()`, por lo que están pensados para **compilarse de a uno** (ver [sección 6](#6-cómo-compilar-y-ejecutar)).

---

## 3. Parte práctica: ejercicios

Todos los archivos se encuentran en `Practico/EDIII_Ejercicios/src/`. En cada tabla se indica el enunciado resumido, los pines utilizados y la técnica principal.

### 3.1 Puertos GPIO y PINSEL

Carpeta: `Puertos GPIO/` · Objetivo: configurar pines (PINSEL, PINMODE, FIODIR, FIOSET/CLR/PIN) y manejar entradas/salidas digitales.

| Archivo | Descripción | Pines | Detalle |
|---|---|---|---|
| `Ejercicio_1_PINSEL.C` | Parpadeo de los colores del **LED RGB** de la placa | R: P0.22 · G: P3.25 · B: P3.26 | LEDs activos por bajo; funciones `configPines()` y `delay()`; tiempo de retardo definido con `#define`; retardo por software |
| `Ejercicio_2_PINSEL.C` | Dos **secuencias de colores** cíclicas (A y B) con tres colores cada una | P0.22–P0.25, P3.25, P3.26 | Alterna `secA` → `secB` con retardo corto entre colores y retardo largo entre secuencias |
| `Ejercicio_3_PINSEL.C` | **Display de 7 segmentos**: cuenta automática de 0 a F | P2.2–P2.8 | Tabla de búsqueda (*lookup table*) con los 16 símbolos hexadecimales |
| `Ejercicio_4_PINSEL.C` | Lee todo el **Puerto 0**, cuenta cuántos pines están en alto y muestra el resultado en binario | Entrada: P0 · Salida: P2.0–P2.4 (5 LEDs) | Recorrido bit a bit sobre `FIOPIN` |
| `Ejercicio_5_PINSEL.C` | **Contador hexadecimal 0–F** con pulsador y display de 7 segmentos | Botón: P2.0 (pull-down) · Display: P0.0–P0.6 · Habilitación: P0.22 | **Antirrebote** por software, detección de flanco y espera a que se suelte el botón; módulo 16 |
| `Ejercicio_6_PINSEL.C` | Lee un valor de **4 bits** (0–F) y lo decodifica en un display de 7 segmentos | Entrada: P0.0–P0.3 · Salida: P2.0–P2.6 | Decodificación mediante tabla de símbolos |
| `Ejercicio_7_PINSEL.C` | Mini **ALU**: dos operandos de 4 bits (A y B) y un interruptor que elige **suma o resta** | A: P0.0–P0.3 · B: P2.0–P2.3 · Operación: P0.4 · Resultado: P0.22–P0.25 · Flags: P0.5 y P0.6 | Resultado en valor absoluto en 4 LEDs; LED adicional para *overflow* (suma) o resultado negativo (resta); diseño con **máquina de estados** (`WAIT_A`, `WAIT_B`, `WAIT_OP`, `EXCE`) |
| `Ejercicio_8_PINSEL.C` | *(archivo vacío, pendiente)* | – | – |
| `Ejercicio_10_PINSEL.C` | *(archivo vacío, pendiente)* | – | – |

### 3.2 GPIO: polling e interrupciones

Carpeta: `Interrupciones GPIO/` · Los ejercicios 1 a 6 trabajan con **polling** sobre GPIO; del 7 al 9 se introducen las **interrupciones por GPIO** (línea compartida `EINT3`).

| Archivo | Descripción | Pines | Detalle |
|---|---|---|---|
| `Ejercicio_1_GPIO.C` | Parpadeo de un LED | LED: P0.22 | Configuración básica y retardo por software |
| `Ejercicio_2_GPIO.C` | Parpadeo de LED con **dos velocidades** según el estado de un pulsador | LED: P0.22 · Botón: P2.10 (pull-up) | Lectura de `FIOPIN` en el lazo principal |
| `Ejercicio_3_GPIO.C` | **Display de 7 segmentos** contando de 0 a 9 | Datos: P2.0–P2.7 · Control: P2.10 | Tabla de búsqueda `lutDSPL_CC` (cátodo común) |
| `Ejercicio_4_GPIO.C` | **Contador binario de 4 LEDs** (0–15) | P0.4–P0.7 | Tabla con los 16 valores binarios |
| `Ejercicio_5_GPIO.C` | LED controlado por **dos pulsadores**, uno con pull-up y otro con pull-down | LED: P0.22 · B1: P2.10 · B2: P2.11 | Sin botones: apagado · solo B1: parpadeo rápido · solo B2: parpadeo lento · ambos: encendido fijo |
| `Ejercicio_6__GPIO.C` | **Luz que se desplaza** de ida y vuelta (efecto "Auto Fantástico") sobre 6 LEDs | P2.0–P2.5 | Recorrido ascendente y descendente en bucle infinito |
| `Ejercicio_7_GPIO.C` | **Toggle de un LED** al detectar un flanco descendente | LED: P0.22 · Botón: P0.15 | Primera **interrupción por GPIO** (`IO0IntEnF`), ISR `EINT3_IRQHandler`, `__WFI()` en el `main` |
| `Ejercicio_8_GPIO.C` | LED que **se enciende al presionar y se apaga al soltar** | LED: P0.22 · Botón: P2.10 (pull-up) | Interrupción por **ambos flancos** (`IO2IntEnF` + `IO2IntEnR`); distingue flanco en la ISR con `IO2IntStatF/R` |
| `Ejercicio_9_GPIO.C` | Dos botones cambian la **velocidad de parpadeo** (rápido/lento) | LED: P0.22 · A: P2.0 (flanco ascendente) · B: P2.1 (flanco descendente) | Uso de macros `SET_MASK`, `CLEAR_MASK` y `TOGGLE_MASK` para manipular registros de forma legible |

### 3.3 Interrupciones externas

Carpeta: `Interrupciones Externas/` · Interrupciones dedicadas `EINT0–EINT3` (`LPC_SC->EXTMODE`, `EXTPOLAR`, `EXTINT`) y comparación con interrupciones por GPIO.

| Archivo | Descripción | Pines | Detalle |
|---|---|---|---|
| `Ejercicio_1_EXINT.c` | **Toggle de LED** con **EINT0** por flanco de bajada | Botón: P2.10 (EINT0) · LED: P0.22 | Configura `EXTMODE`/`EXTPOLAR`, ISR `EINT0_IRQHandler`, limpia bandera en `EXTINT` |
| `Ejercicio_1_EINT3.c` | Mismo ejercicio resuelto con la **interrupción GPIO** (línea EINT3) | P2.10 · P0.22 | Variante para comparar ambos mecanismos |
| `Ejercicio_2_EINT3.c` | **Contador hexadecimal 0–F** en display de 7 segmentos incrementado por interrupción de **flanco de subida** | Pulsador: P0.0 · Display: 7 pines | La lógica de incremento vive dentro de la ISR |
| `Ejercicio_3_EINT3.c` / `Ejercicio_3_EXINT.c` | Contador en display donde una interrupción externa controla el **sentido de la cuenta** (ascendente/descendente) | Display + pulsador | Versión con interrupción GPIO (EINT3) y versión con EINT0 |
| `Ejercicio_4_EXINT.C` | **Dos fuentes de interrupción con prioridades distintas** | P0.0 (GPIO, flanco de subida) · P2.11 (EINT1, flanco de bajada) · LEDs: P0.2–P0.5 y P2.0–P2.3 | `NVIC_SetPriority`: EINT1 tiene **mayor prioridad** que la interrupción GPIO (EINT3); cada ISR ejecuta una secuencia de parpadeo distinta |

### 3.4 SysTick

Carpeta: `SysTick/` · Temporizador del sistema del Cortex-M3 (`SysTick->CTRL/LOAD/VAL`).

| Archivo | Descripción | Detalle |
|---|---|---|
| `Ejercicio_1_SysTick.C` | Configura el SysTick para interrumpir cada **10 ms** y escribe el handler mínimo | `LOAD = 999999` con CCLK = 100 MHz; el handler conmuta el LED P0.22 |
| `Ejercicio_2_SysTick.C` | **Parpadeo de LED cada 500 ms** usando SysTick | El `main` queda en un bucle vacío con `__WFI()`: toda la tarea se gestiona por interrupción |

### 3.5 Timer 0

Carpeta: `Timer0/` · Uso de los **canales Match** del Timer 0 con los drivers `lpc17xx_timer.h`, base de tiempo de **1 µs por tick** (prescaler para PCLK = 25 MHz).

| Archivo | Descripción | Concepto |
|---|---|---|
| `Ejercicio_1_TM0.c` | Configurar la **base de tiempo básica**: 1 tick de TC = 1 µs | Jerarquía de relojes CCLK → PCLK → prescaler |
| `Ejercicio_2_TM0.c` | **Interrupción periódica cada 250 ms** que conmuta un LED en P0.22 | MR0 con `MR0I = 1`, `MR0R = 1`, `MR0S = 0` |
| `Ejercicio_3_TM0.c` | Interrupción periódica cada **50 ms** (esqueleto reutilizable) | Match 0 con reinicio automático |
| `Ejercicio_3.c` | **Onda cuadrada de 1 kHz** en el pin MAT0.0 (P1.28) | Semiperiodo de 500 µs, MR0 = 499, sin carga para la CPU |
| `Ejercicio_4_TM0.c` | **One-shot**: intervalo exacto de 3 s tras el cual el timer se detiene solo | `MR0I`, `MR0R` y `MR0S` en 1 |
| `Ejercicio_5_TM0.c` | **Generador de onda cuadrada autónomo** de 1 kHz | Modo `TIM_TOGGLE` en el registro EMR, interrupción deshabilitada |
| `Ejercicio_6_TM0.c` | **Pulso asimétrico** generado por hardware con MR0 (SET) y MR1 (CLEAR) | Combinación de dos canales Match: 10 ms de bajo y 30 ms de alto |
| `Ejercicio_7_TM0.c` | **Disparo por hardware del ADC** con MAT0.1 a 10 Hz | MAT0.1 en modo toggle como fuente de inicio de conversión (`ADC_START_ON_MAT01`, flanco descendente) |

### 3.6 ADC

Carpeta: `ADC/` · Conversor analógico-digital de 12 bits, con disparo por software, por hardware (Timer) y modo ráfaga.

| Archivo | Descripción | Detalle |
|---|---|---|
| `Ejercicio_1_ADC.c` | **Voltímetro básico**: LED en P0.22 encendido si la tensión supera **2,0 V** | Timer 0 interrumpe cada 100 ms y dispara la conversión por software (`ADC_START_NOW`); el umbral de 2,0 V equivale a ≈ 2482 cuentas |
| `Ejercicio_2_ADC.c` | **Monitoreo de temperatura** (LM35) con alarma si supera **40 °C** | Disparo automático por hardware con MAT0.1 (P1.29, toggle cada 500 ms); conversión cuentas → mV → °C (10 mV/°C); la CPU permanece dormida hasta `ADC_IRQHandler` |
| `Ejercicio_3_1_ADC.c` | Digitalización de **dos señales de 10 kHz de ancho de banda** (canales 2 y 4) en **modo manual** | Buffers circulares de 20 muestras en regiones distintas de SRAM (`0x2007C000`); se respeta Nyquist |
| `Ejercicio_6.1.c` | **Alarma a 60 °C** con un sensor LM35 en AD0.0 | Muestreo de 1 Hz por hardware (MAT0.1); alarma visual en P0.22 |
| `Ejercicio_6_2.c` | Igual que el anterior con **dos sensores LM35** (AD0.0 y AD0.1) | Alternancia de canales: al terminar AD0.0, se inicia la conversión de AD0.1 |

### 3.7 DMA (GPDMA)

Carpeta: `DMA/` · Controlador DMA de propósito general, con listas enlazadas (*linked lists*) y buffers circulares.

| Archivo | Descripción | Detalle |
|---|---|---|
| `Ejercicio_1_GPDMA.C` | **DAC + GPDMA**: reproduce por P0.26 una señal guardada en memoria | Transferencia memoria → periférico (`GPDMA_M2P`), palabras de 16 bits, 256 muestras, DAC con DMA y *time-out* de muestreo |
| `Ejercicio_2_GPDMA.C` | **Grabación y reproducción de señales**: el ADC (modo burst) guarda durante **1 minuto** en un buffer circular y luego el DAC lo reproduce durante otro minuto | Integra ADC + DAC + Timer + GPDMA; canal 0 (P2M, ADC → memoria) y canal 1 (M2P, memoria → DAC) con listas enlazadas; el Timer 0 (60 s) alterna entre "escribiendo" y "leyendo" |
| `Ejercicio_Integrador1.c` | **Ejercicio integrador** (esqueleto): combina EINT0, Timer 0, ADC y GPDMA | Configuración inicial de pines, interrupción externa, timer y ADC |

### 3.8 Parciales

Carpeta: `Parciales/` · Resolución de enunciados de parciales de distintos años.

| Archivo | Enunciado resumido | Conceptos |
|---|---|---|
| `Parcial_1_A.c` | Contador **0 a 9** con **SysTick** y tres interrupciones externas: EINT0 reinicia y mantiene en 0 mientras está presionado, EINT1 pausa/continúa y EINT2 alterna la velocidad entre **1 s y 1 ms** (CCLK = 80 MHz) | SysTick (1 ms), EINT0 por nivel, EINT1/EINT2 por flanco, prioridades en el NVIC, display de 7 segmentos en P2.0–P2.6 |
| `Parcial_1_B.c` | Generador de **dos señales cuadradas de 50 Hz** por software con **desfasaje dinámico** (0°, 90°, 180°, 270°) controlado por EINT0 | SysTick a 1 ms (CCLK = 100 MHz), señal A en P0.15 y señal B en P0.16, máquina de estados con tabla de offsets {0, 5, 10, 15} ms |
| `Parcial 2022/Ejercicio1_parcial_2022.c` | **Señal triangular** simétrica con Timer 0 y DAC, con el menor período, la máxima excursión y el menor incremento posibles | DAC de 10 bits (0–1023), `DAC_SetBias(0)` para máxima velocidad, interrupción por Match |
| `Parcial 2022/Ejercicio2_parcial_2022.c` | Digitalizar **dos señales de 10 kHz** (canales 2 y 4) y guardar los **últimos 20 datos** de cada una (modo **burst**) | ADC en ráfaga a 40 kHz, buffers circulares con punteros en SRAM, interrupciones por canal |
| `Parciales 2023/Ejercicio1_parcial_2023.c` | Sensor de temperatura en el ADC: una muestra cada **30 s**, a los 2 min se **promedian 4 muestras** y se decide la salida: < 1 V → 0 · entre 1 y 2 V → **PWM de 20 kHz** con ciclo de trabajo de 50 % a 90 % · > 2 V → 1 | Timer 0 (MAT0.1) dispara el ADC; Timer 1 genera el PWM por software con MR0 (período de 50 µs) y MR1 (ciclo de trabajo); ecuación de la recta para el duty |
| `Parciales 2023/Ejercicio2_parcial_2023.c` | Tres bloques de 4 KB con formas de onda; con **DMA + DAC** se genera una salida distinta en cada interrupción externa (60 kHz, 120 kHz, 450 kHz, y vuelta a empezar), minimizando el consumo del DAC | Cuatro interrupciones externas (`EXTI`), bloques en SRAM, esqueleto de configuración de DAC y DMA |
| `Parcial 2025/Ejercicio_1.C` | Usar **GPDMA** para copiar datos (palabras de 2 bytes) desde la primera mitad de la SRAM del banco 1 hacia la segunda mitad del banco 0, y encender un LED (P0.22) al terminar | Transferencia **M2M**, interrupción por **cuenta terminal** (`GPDMA_INTTC`) y por error |
| `Parcial 2025/Ejercicio_2.C` | Variante del ejercicio de temperatura del 2023: una muestra cada 30 s y promedio de las últimas 4 muestras para decidir el estado de una salida digital | Prescaler de 1 ms, Match en 29000, buffer de 4 muestras, cálculo de promedio en mV |

---

## 4. Librería CMSIS y drivers

El proyecto `Practico/CMSISv2p00_LPC17xx/` es la librería base que utiliza `EDIII_Ejercicios` (declarada como dependencia en el `.project`, con los símbolos `__USE_CMSIS=CMSISv2p00_LPC17xx` y `__LPC17XX__`). Incluye:

- **CMSIS Core:** `core_cm3.c` y `system_LPC17xx.c`.
- **Drivers de periféricos NXP** (`Drivers/inc` y `Drivers/src`): `gpio`, `pinsel`, `timer`, `systick`, `exti`, `nvic`, `adc`, `dac`, `gpdma`, `uart`, `i2c`, `i2s`, `spi`, `ssp`, `can`, `emac`, `pwm`, `mcpwm`, `qei`, `rit`, `rtc`, `wdt`, `clkpwr`, `libcfg_default` y `debug_frmwrk`.
- **Tests unitarios** de varios drivers (`Drivers/tests`).

Además, en `EDIII_Ejercicios/src/` se encuentran `cr_startup_lpc175x_6x.c` (tabla de vectores y arranque del micro) y `crp.c` (protección de lectura del código).

---

## 5. Parte teórica

La carpeta `Teorico/` agrupa el material de estudio:

- **`Clases/`** – Apuntes de las clases del Ing. E. Migliore: `EDIII_Clase_1A`, `1B` y de la `2` a la `8` (PDF).
- **`Documentación_LPC1769/`**
  - Datasheet del LPC1769.
  - User Manual del LPC176x/5x (también versión para imprimir).
  - Esquemático de la placa LPCXpresso LPC1769 Rev. B.
  - Documento de referencia de los drivers (*Drivers – 2026*).
  - Tablas de registros: *Tablas LPC 1769 – Hoja 1* y *Tabla EDIII primer parcial*.
- **`Ejercicios/`** – Enunciados propuestos para el taller (`ED3 – Ejercicios propuestos (taller)`) y las guías de ejercicios `2_x`, `3_x`, `4_x`, `5_1_1`, `6_x_x`, `8_x` y `A_x`.

---

## 6. Cómo compilar y ejecutar

### Requisitos
- **MCUXpresso IDE** (o LPCXpresso) con el toolchain ARM GCC.
- Placa **LPCXpresso LPC1769** conectada por USB (sonda de depuración integrada).

### Pasos

1. Clonar el repositorio:
   ```bash
   git clone https://github.com/rafa2599/Ejercicios_LPC1769.git
   ```
2. En MCUXpresso: **File → Import → General → Existing Projects into Workspace** y seleccionar la carpeta `Practico/`. Se importan dos proyectos:
   - `CMSISv2p00_LPC17xx` (librería)
   - `EDIII_Ejercicios` (ejercicios)
3. **Compilar primero** `CMSISv2p00_LPC17xx` (genera `libCMSISv2p00_LPC17xx.a`) y luego `EDIII_Ejercicios`.
4. **Elegir qué ejercicio ejecutar.** Como cada archivo tiene su propio `main()`, solo uno puede estar activo a la vez. Opciones:
   - Clic derecho sobre el archivo → **Resource Configurations → Exclude from Build** para excluir los demás, o
   - Dejar habilitado únicamente el ejercicio que se quiere probar.
5. Conectar la placa y ejecutar **Debug** (icono del bicho). El binario se carga y se depura mediante la sonda integrada.

---

## 7. Conceptos y técnicas utilizadas

| Tema | Dónde se aplica |
|---|---|
| Multiplexado de pines (`PINSEL`) y resistencias pull-up/pull-down (`PINMODE`) | Todos los ejercicios de GPIO |
| Registros GPIO rápidos (`FIODIR`, `FIOSET`, `FIOCLR`, `FIOPIN`, `FIOMASK`) | Puertos GPIO e Interrupciones GPIO |
| Tablas de búsqueda para display de 7 segmentos | Ejercicios 3, 5 y 6 de PINSEL, Interrupciones GPIO y Externas |
| Antirrebote (*debounce*) por software y detección de flancos | `Ejercicio_5_PINSEL.C` |
| Máquinas de estados | `Ejercicio_7_PINSEL.C`, `Parcial_1_B.c` |
| Interrupciones por GPIO y compartición de la línea `EINT3` | Interrupciones GPIO / Externas |
| Interrupciones externas dedicadas `EINT0–2` (modo flanco/nivel y polaridad) | Interrupciones Externas, Parcial 1 A y B |
| Prioridades en el NVIC | `Ejercicio_4_EXINT.C`, `Parcial_1_A.c` |
| Modo de bajo consumo con `__WFI()` | Casi todos los ejercicios con interrupciones |
| SysTick como base de tiempo | SysTick, Parcial 1 A y B |
| Timer: prescaler, registros Match (MR), MCR y EMR | Timer0, ADC, DMA y Parciales |
| Generación de señales por hardware (modo toggle, set/clear) | `Ejercicio_3.c`, `5`, `6` y `7` de Timer0 |
| PWM por software con dos timers | `Ejercicio1_parcial_2023.c` |
| ADC: disparo por software, por hardware (MAT0.1) y modo burst | Carpeta ADC y Parciales |
| Conversión de cuentas a tensión y temperatura (LM35: 10 mV/°C) | ADC y Parciales |
| Buffers circulares en SRAM | `Ejercicio_3_1_ADC.c`, `Ejercicio2_parcial_2022.c`, DMA |
| DAC (generación de señales, bias de bajo consumo) | `Parcial 2022`, DMA, `Parcial 2023` |
| GPDMA: M2M, M2P, P2M, listas enlazadas, interrupción por TC/error | Carpeta DMA y Parciales 2023/2025 |

---

## 8. Estado del repositorio y mejoras pendientes

Este repositorio refleja el **trabajo en progreso** de un curso, por lo que conviven ejercicios terminados con borradores. Para tener una visión clara:

**Ejercicios completos y funcionales (a nivel de lógica):** `Ejercicio_5_PINSEL`, la mayoría de las carpetas `Interrupciones GPIO` e `Interrupciones Externas`, `SysTick/Ejercicio_1`, `Timer0` (2, 3, 5), `ADC` (1, 2), `Parcial_1_A`, `Parcial_1_B`, `Parcial 2022` (1 y 2), `Parciales 2023/Ejercicio1` y `Parcial 2025/Ejercicio_1`.

**Archivos vacíos o esqueletos para completar:**
- `Puertos GPIO/Ejercicio_8_PINSEL.C` y `Ejercicio_10_PINSEL.C` (vacíos).
- `Parciales/aa.c` (vacío).
- `DMA/Ejercicio_Integrador1.c`, `Parciales 2023/Ejercicio2_parcial_2023.c` y `Parcial 2025/Ejercicio_2.C` (estructura iniciada, lógica pendiente).
- `Puertos GPIO/Ejercicio_7_PINSEL.C` (ALU en desarrollo).

**Puntos a revisar antes de compilar (observaciones detectadas al leer el código):**
- Algunos nombres de handlers no coinciden con los de la tabla de vectores del startup. Los nombres correctos son `SysTick_Handler`, `TIMER0_IRQHandler` y `EINT3_IRQHandler`; por ejemplo, `SysTick_IRQHandler`, `Timer0_IRQHandler` o `EINT3_IRQnHandler` no serían invocados.
- Hay pequeños errores de tipeo que impiden compilar en algunos archivos (por ejemplo `uint23_t`, `simbols`, `unint32_t`, `prtAdc`, `LPC_GPI0`) y operadores `>` usados en lugar de `<<` en máscaras como `(1>22)`.
- Algunos retardos por software usan constantes mayores a `UINT32_MAX` (por ejemplo `3000000000`); conviene ajustarlas.
- Los archivos `.c` con extensión en mayúscula (`.C`) pueden requerir revisión según la configuración del proyecto.

**Sugerencias de mejora del repositorio:**
- Agregar a `.gitignore` las carpetas `Debug/` y `Release/` (actualmente se versionan archivos compilados `.o`, `.d`, `.su` y `.a` dentro de `CMSISv2p00_LPC17xx/Debug`).
- Agregar descripción y *topics* (`lpc1769`, `embedded-c`, `cortex-m3`, `mcuxpresso`) al repositorio en GitHub.
- Incorporar esquemas de conexión o fotos de los montajes en una carpeta `docs/`.
- Unificar la convención de nombres de archivos y carpetas (por ejemplo, `Ejercicio_6__GPIO.c` y `Ejercicio_6.1.c`).

---

## Autor

**Rafael** ([@rafa2599](https://github.com/rafa2599)) – Estudiante de Electrónica Digital III.

> Material teórico y enunciados: cátedra de Electrónica Digital III (Ing. E. Migliore). Los drivers y la librería CMSIS pertenecen a **NXP Semiconductors** y se incluyen únicamente con fines académicos.
