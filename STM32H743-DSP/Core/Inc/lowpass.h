/*
 * lowpass.h
 *
 *  Created on: Feb 27, 2025
 *      Author: spencer
 */

#ifndef LOWPASS_FILTER_H
#define LOWPASS_FILTER_H


#include <stdint.h>
#include <math.h>

#define __FPU_PRESENT  1U
#define ARM_MATH_CM7
#include "arm_math.h"
// Structure to hold the biquad low-pass filter state and coefficients.
typedef struct {
    float32_t b0, b1, b2;
    float32_t a1, a2;
    // Delay elements for input and output
    float32_t x1, x2;
    float32_t y1, y2;
} LowPassFilter;

// Initialize the low-pass filter for cutoff frequency fc (Hz) and sample rate fs (Hz).
// For a Butterworth filter, fc = 10kHz (or any desired value).
void lowPassFilterInit(LowPassFilter *filter, float32_t fc, uint32_t fs);

// Process a block of samples through the low-pass filter.
// 'input' is the pointer to the input array,
// 'output' is the pointer to the output array (can be the same as input for in-place processing),
// 'blockSize' is the number of samples in the block.
void lowPassFilterProcess(LowPassFilter *filter, float32_t *input, float32_t *output, uint32_t blockSize);

#endif /* LOWPASS_FILTER_H */
