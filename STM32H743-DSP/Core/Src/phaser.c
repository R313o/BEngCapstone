/*
 * phaser.c
 *
 *  Created on: Mar 11, 2025
 *      Author: Federico
 */

#include "phaser.h"

void phaserInit(phaser *p, float32_t wetness, float32_t depth, float32_t rate, uint8_t stages, uint32_t frameLength, uint32_t sampleRate) {
    p->wetness = wetness;
    p->depth = depth;
    p->rate = rate;
    p->stages = stages;

    // Allocate delay buffers for each stage
    p->inDelayed = (float32_t *)malloc(stages * sizeof(float32_t));
    p->outDelayed = (float32_t *)malloc(stages * sizeof(float32_t));

    // Initialize buffers to zero
    for (uint8_t i = 0; i < stages; i++) {
        p->inDelayed[i] = 0.0f;
        p->outDelayed[i] = 0.0f;
    }

    // Initialize LFO time
    p->t = 0.0f;
    p->dt = 1.0f / (float32_t)sampleRate;
}


void phaserApply(phaser *p, pipe *pipe) {
    for (uint32_t n = 0; n < BUFFER_SIZE; n++) {
        float32_t input = pipe->processBuffer[n];

        // Compute LFO modulation (-1 to 1)
        float32_t lfo = sinf(2.0f * M_PI * p->rate * p->t);

        // Map LFO to G coefficient
        float32_t G = p->depth * lfo;

        float32_t dry = input;
        float32_t filtered = input;

        // Process through all-pass filter stages
        for (uint8_t stage = 0; stage < p->stages; stage++) {
            float32_t x = filtered;
            float32_t y = p->inDelayed[stage] - G * x + G * p->outDelayed[stage];

            // Update delay buffers
            p->inDelayed[stage] = x;
            p->outDelayed[stage] = y;

            // Pass output to next stage
            filtered = y;
        }

        // Mix dry and wet signal
        pipe->processBuffer[n] = dry + p->wetness * filtered;

        // Advance LFO time
        p->t += p->dt;
    }
}
