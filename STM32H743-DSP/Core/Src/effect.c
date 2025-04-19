/*
 * effect.c
 *
 *  Created on: Feb 27, 2025
 *      Author: spencer
 */

#include "effect.h"
#include "pipe.h"  // Needed for definition of pipe and BUFFER_SIZE

// Default processing function: applies the gain to each sample in the pipe's processBuffer.
static void defaultProcess(effect *self, pipe *p) {
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        p->processBuffer[i] *= self->gain;
    }
}

void effectInit(effect *self, float32_t gain) {
    self->gain = gain;
    self->process = defaultProcess;
}

void effectApply(effect *self, pipe *p) {
    if (self->process) {
        self->process(self, p);
    }
}
