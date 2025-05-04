/*
 * process.h
 *
 *  Created on: May 2, 2025
 *      Author: spencer
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stddef.h>
#include "dataLink.h"

#define BUFFER_SIZE 8
#define MAX_INPUTS  4   /* keep in sync with dataLink.h */

/* ------------------------------------------------------------------ */
/*  I/O pipe definition                                               */
/* ------------------------------------------------------------------ */
typedef struct pipeDef
{
    float in1[BUFFER_SIZE];
    float in2[BUFFER_SIZE];
    float out1[BUFFER_SIZE];
    float out2[BUFFER_SIZE];
} pipeDef;

/* effect helpers (exposed for unit‑tests) */
void addEffect      (const float *in, float *out, int p);
void subtractEffect (const float *in, float *out, int p);
void multiplyEffect (const float *in, float *out, int p);
void divideEffect   (const float *in, float *out, int p);
void mixEffect      (float *const inputs[], float *out,
                     const float *w, int cnt);

/* Run the DSP network once.
 *  link → describes the graph
 *  pipe → holds input and receives output
 *  len  → number of samples (<= BUFFER_SIZE)
 */
void processNetwork(dataLink *link, pipeDef *pipe, size_t len);

#endif /* PROCESS_H */

