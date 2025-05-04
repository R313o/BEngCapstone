/*
 * phaser.h
 *
 *  Created on: March 11, 2025
 *      Author: Federico Sarabia
 */

#ifndef PHASER_H_
#define PHASER_H_

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

typedef struct phaser {
    uint32_t baseDelay;   // Base delay (N), for phaser it is always set to 1
    float32_t wetness;    // Wetness ratio (W)
    float32_t depth;      // Modulation depth
    float32_t rate;       // LFO rate in Hz
	uint8_t stages; 	  // Number of stages

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
} phaser;



// And function prototypes:
void phaserInit(phaser *p, float32_t wetness, float32_t depth, float32_t rate, uint8_t stages, uint32_t frameLength, uint32_t sampleRate);
void phaserApply(phaser *c, pipe *p);

#endif /* PHASER_H_ */
