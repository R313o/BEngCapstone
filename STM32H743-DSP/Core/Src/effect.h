/*
 * effect.h
 *
 *  Created on: Feb 27, 2025
 *      Author: spencer
 */

#ifndef SRC_EFFECT_H_
#define SRC_EFFECT_H_

#include <stdint.h>

#define __FPU_PRESENT  1U
#define ARM_MATH_CM7

#include "arm_math.h"


struct pipe;
typedef struct pipe pipe;

// Define the effect structure.
typedef struct effect effect;

// Function pointer type for processing a pipe.
typedef void (*EffectProcessFn)(effect *self, pipe *p);

struct effect {
    float32_t gain;
    EffectProcessFn process;
};

// Initializes the effect with a given gain value.
void effectInit(effect *self, float32_t gain);

// Applies the effect to the process buffer inside the given pipe.
void effectApply(effect *self, pipe *p);


#endif /* SRC_EFFECT_H_ */
