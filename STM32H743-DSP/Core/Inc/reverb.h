/*
 * reverb.h
 *
 *  Created on: Mar 4, 2025
 *      Author: spencer
 */

#ifndef SRC_REVERB_H_
#define SRC_REVERB_H_

#include <stdint.h>

#define __FPU_PRESENT  1U
#define ARM_MATH_CM7

#include "arm_math.h"
#include "pipe.h"


// Configuration parameters.
// BUFFER_SIZE is the number of samples per input block.
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 512
#endif
// We choose FFT_SIZE = 2 * BUFFER_SIZE.
#define FFT_SIZE (2 * BUFFER_SIZE)
// We set MAX_IR_LENGTH such that the entire IR and all temporary buffers never exceed 4×BUFFER_SIZE.
// For example, we limit the IR to 2048 samples.
#define MAX_IR_LENGTH (2048)

// The reverb structure implements overlap–save convolution for an IR up to MAX_IR_LENGTH samples.
// It precomputes and stores the FFT of the IR (padded to FFT_SIZE) in a static array.
// It maintains an overlap buffer (of length FFT_SIZE - BUFFER_SIZE) for the input.
typedef struct reverb {
    float32_t mix;            // Wet/dry mix: 0.0 = dry, 1.0 = fully wet
    const float32_t *ir;      // Pointer to the normalized IR array (should be <= MAX_IR_LENGTH)
    uint32_t irLength;        // Actual IR length used (<= MAX_IR_LENGTH)

    // Precomputed FFT of the IR, stored in an array of length FFT_SIZE.
    float32_t irFFT[FFT_SIZE];

    // Overlap buffer: stores FFT_SIZE - BUFFER_SIZE samples for the next block.
    float32_t overlap[FFT_SIZE - BUFFER_SIZE];

    // FFT instance for FFT_SIZE.
    arm_rfft_fast_instance_f32 fftInstance;
} reverb;

// Initializes the reverb effect using overlap–save convolution.
// The IR is assumed to come from an external file (e.g. lexiconIR.h) as a normalized float32_t array.
// Only the first irLength samples (<= MAX_IR_LENGTH) are used.
// 'mix' is the wet/dry mix (0.0 to 1.0).
void reverbInit(reverb *self, float32_t mix, const float32_t *ir, uint32_t irLength);

// Applies the reverb effect to the pipe's processBuffer (of length BUFFER_SIZE).
// This function performs the overlap–save convolution and mixes the wet result with the dry signal.
void reverbApply(reverb *self, pipe *p);

#endif /* SRC_REVERB_H_ */
