
/*  config.h  ---------------------------------------------------------
 *  Central location for common project‑wide constants.
 *  ------------------------------------------------------------------ */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stddef.h>


#define BUFFER_SIZE    1024
#define DELAY_BUFFER_SIZE (BUFFER_SIZE * 10)

#define ADC_BITS2VOLTS 0.000030517578f
#define DAC_VOLTS2BITS 2048.0f


#define NUM_PARAMETERS 3
#define MAX_INPUTS     3
#define MAX_NODES      10


#define VECTOR_SIZE    BUFFER_SIZE
#define UART_BUFFER_SIZE 128

#endif /* CONFIG_H */
