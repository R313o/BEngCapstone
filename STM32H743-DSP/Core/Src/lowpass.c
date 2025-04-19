/*
 * lowpass.c
 *
 *  Created on: Feb 27, 2025
 *      Author: spencer
 */

#include "lowpass.h"

// The initialization uses a bilinear transform on a second-order Butterworth prototype.
// It computes:
//   K = tan(pi*fc/fs)
//   norm = 1/(1 + sqrt(2)*K + K*K)
//   b0 = K*K * norm
//   b1 = 2*b0
//   b2 = b0
//   a1 = 2*(K*K - 1)*norm
//   a2 = (1 - sqrt(2)*K + K*K)*norm
void lowPassFilterInit(LowPassFilter *filter, float32_t fc, uint32_t fs) {
    float32_t K = tanf(M_PI * fc / (float32_t)fs);
    float32_t norm = 1.0f / (1.0f + 1.41421356f * K + K * K);
    filter->b0 = K * K * norm;
    filter->b1 = 2.0f * filter->b0;
    filter->b2 = filter->b0;
    filter->a1 = 2.0f * (K * K - 1.0f) * norm;
    filter->a2 = (1.0f - 1.41421356f * K + K * K) * norm;

    // Initialize delay elements to zero.
    filter->x1 = filter->x2 = 0.0f;
    filter->y1 = filter->y2 = 0.0f;
}

void lowPassFilterProcess(LowPassFilter *filter, float32_t *input, float32_t *output, uint32_t blockSize) {
    for (uint32_t n = 0; n < blockSize; n++) {
        float32_t x = input[n];
        float32_t y = filter->b0 * x + filter->b1 * filter->x1 + filter->b2 * filter->x2
                      - filter->a1 * filter->y1 - filter->a2 * filter->y2;
        output[n] = y;

        // Shift delay buffers
        filter->x2 = filter->x1;
        filter->x1 = x;
        filter->y2 = filter->y1;
        filter->y1 = y;
    }
}

