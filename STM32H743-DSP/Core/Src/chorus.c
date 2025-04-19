/*
 * chorus.c
 *
 *  Created on: Feb 27, 2025
 *      Author: spencer
 */

#include "chorus.h"

void chorusInit(chorus *c, uint32_t baseDelay, float32_t wetness, float32_t depth, float32_t rate, uint32_t frameLength, uint32_t sampleRate) {
    c->baseDelay = baseDelay;
    c->wetness = wetness;
    c->depth = depth;
    c->rate = rate;
    c->G_fb = 0.5f;
    c->G_ff = 1.0f;

    // Compute a suitable delay buffer size:
    uint32_t size1 = frameLength * 5;
    uint32_t size2 = (uint32_t)(ceilf((float32_t)baseDelay * (1.0f + depth)) * 2);
    c->bufferSize = (size1 > size2) ? size1 : size2;
    if (c->bufferSize < frameLength) {
        c->bufferSize = frameLength;
    }

    // Allocate the delay buffers (mono processing)
    c->inDelayed = (float32_t *)malloc(c->bufferSize * sizeof(float32_t));
    c->outDelayed = (float32_t *)malloc(c->bufferSize * sizeof(float32_t));
    for (uint32_t i = 0; i < c->bufferSize; i++) {
        c->inDelayed[i] = 0.0f;
        c->outDelayed[i] = 0.0f;
    }

    c->writePtr = 0;
    c->t = 0.0f;
    c->dt = 1.0f / (float32_t)sampleRate;
}

void chorusApply(chorus *c, pipe *p) {
    // Process each sample in the frame (assumed to be BUFFER_SIZE samples)
    for (uint32_t n = 0; n < BUFFER_SIZE; n++) {
        float32_t input = p->processBuffer[n];

        // Compute current delay (in samples) with modulation via an LFO.
        float32_t mod = sinf(2.0f * M_PI * c->rate * c->t);
        uint32_t currentDelay = (uint32_t)(roundf(c->baseDelay * (1.0f + c->depth * mod)));

        // Compute the read pointer in the circular delay buffer.
        uint32_t readPtr;
        if (currentDelay >= c->bufferSize) {
            readPtr = c->writePtr;  // Clamp if delay is too high
        } else {
            readPtr = (c->writePtr + c->bufferSize - currentDelay) % c->bufferSize;
        }

        // Compute output sample using a mix of dry and delayed signals.
        float32_t delayedInput = c->inDelayed[readPtr];
        float32_t delayedOutput = c->outDelayed[readPtr];
        float32_t output = (1.0f - c->wetness) * input + c->wetness * (c->G_fb * delayedOutput + (c->G_ff - c->G_fb) * delayedInput);

        // Update delay buffers at the current write pointer.
        c->inDelayed[c->writePtr] = input;
        c->outDelayed[c->writePtr] = output;

        // Write the processed sample back into the processBuffer.
        p->processBuffer[n] = output;

        // Advance the circular buffer pointer.
        c->writePtr = (c->writePtr + 1) % c->bufferSize;
        // Advance time by dt.
        c->t += c->dt;
    }
}

