/*
 * chorus.h
 *
 *  Created on: Feb 27, 2025
 *      Author: spencer
 */

#ifndef SRC_CHORUS_H_
#define SRC_CHORUS_H_


#include "pipe.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define __FPU_PRESENT  1U
#define ARM_MATH_CM7

#include "arm_math.h"
#ifndef M_PI
  #define M_PI 3.14159265358979323846f
#endif

// Chorus effect structure. Parameters:
//   baseDelay: base delay in samples (N)
//   wetness: wet/dry mix (0 = dry, 1 = fully wet)
//   depth: modulation depth (relative change to base delay)
//   rate: LFO rate in Hz
typedef struct chorus {
    uint32_t baseDelay;   // Base delay (N)
    float32_t wetness;    // Wetness ratio (W)
    float32_t depth;      // Modulation depth
    float32_t rate;       // LFO rate in Hz

    // Fixed gains (you can adjust these as desired)
    float32_t G_fb;       // Feedback gain (e.g. 0.5)
    float32_t G_ff;       // Feedforward gain (e.g. 1.0)

    // Internal delay buffers (mono processing)
    // Buffer size is computed as: max(frameLength * 5, ceil(baseDelay*(1+depth)) * 2)
    uint32_t bufferSize;
    float32_t *inDelayed;   // Input delay buffer
    float32_t *outDelayed;  // Output delay buffer

    // Circular buffer pointer
    uint32_t writePtr;

    // LFO time management
    float32_t t;    // Current time (in seconds)
    float32_t dt;   // Time increment = 1/sampleRate
} chorus;

// Initializes the chorus effect.
//   baseDelay: base delay in samples (N)
//   wetness: wet/dry mix ratio (0 to 1)
//   depth: modulation depth (e.g. 0.45)
//   rate: LFO rate in Hz (e.g. 0.05)
//   frameLength: the frame size (typically BUFFER_SIZE)
//   sampleRate: sampling rate in Hz
void chorusInit(chorus *c, uint32_t baseDelay, float32_t wetness, float32_t depth, float32_t rate, uint32_t frameLength, uint32_t sampleRate);

// Applies the chorus effect to the pipe's processBuffer.
void chorusApply(chorus *c, pipe *p);

#endif /* SRC_CHORUS_H_ */
