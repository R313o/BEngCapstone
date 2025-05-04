
/*  config.h  ---------------------------------------------------------
 *  Central location for common project‑wide constants.
 *  ------------------------------------------------------------------ */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stddef.h>

/* Block / buffer length (floats per channel) */
#define BUFFER_SIZE    8

/* Maximums for graphs */
#define NUM_PARAMETERS 3
#define MAX_INPUTS     3
#define MAX_NODES      10

/* Keep legacy alias so existing code that used VECTOR_SIZE still builds */
#define VECTOR_SIZE    BUFFER_SIZE

#endif /* CONFIG_H */
