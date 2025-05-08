/*
 * phaser.c
 *
 *  Created on: Mar 11, 2025
 *      Author: Federico
 */

#include "_MULTI_FX.h"

#define _static_mem_alloc _static_mem_alloc_ram_d2


static void phaser_init_f32(phaser_f32 *self, float32_t *state, phaser_params_t *p) {

    self->state = state;
    self->params = p;

    self->t = 0.0f;
    self->dt = 1.0f / 48000.0f ; // sample rate !magic # for now.
}

static void phaser_f32_process(FX_HANDLER_t *fx, pipe *pipe){

	phaser_f32 *p = fx->states[2];

	float32_t *inDelayed = fx->states[1];
	float32_t *outDelayed = &inDelayed[p->params->stages];


	for (uint32_t n = 0; n < BUFFER_SIZE; n++) {
		float32_t input = pipe->processBuffer[n];

		// Compute LFO modulation (-1 to 1)
		float32_t lfo = sinf(2.0f * M_PI * p->params->rate * p->t);

		// Map LFO to G coefficient
		float32_t G = p->params->depth * lfo;

		float32_t dry = input;
		float32_t filtered = input;

		// Process through all-pass filter stages
		for (uint8_t stage = 0; stage <  p->params->stages ; stage++) {
			float32_t x = filtered;
			float32_t y = inDelayed[stage] - G * x + G * outDelayed[stage];

			// Update delay buffers
			inDelayed[stage] = x;
			outDelayed[stage] = y;

			// Pass output to next stage
			filtered = y;
		}

		// Mix dry and wet signal
		pipe->processBuffer[n] = dry +  p->params->wetness * filtered;

		// Advance LFO time
		p->t += p->dt;
	}
}


void phaser_f32_clean(phaser_f32 *self){

	self->state = NULL;
	self->params  = NULL;

}


void fx_phaser_init(FX_HANDLER_t *fx){


	fx->num_params = 3; // 3 params for a chorus effect

    // allocate parameter instance
    fx->states[0] = _dctm_static_mem_alloc(
        sizeof(phaser_params_t),
        _Alignof(phaser_params_t)
    );

    phaser_params_t *p = (phaser_params_t*)fx->states[0];

    /*
     * defaults
     */
    p->wetness  = 0.5 ;
	p->depth 	= 0.1 ;
	p->rate     = 1.5 ;

    p->stages   = 15   ;

    // Allocate input and output delay state buffers
	fx->states[1] = _static_mem_alloc(
        2 * (p->stages) * sizeof(float),   // (bottom half is input, top half is output)
        _Alignof(float)
    );

    // allocate phaser instance
    fx->states[2] = _dctm_static_mem_alloc(
        sizeof(phaser_f32),
        _Alignof(phaser_f32)
    );

    // Initialize phasor effect
    phaser_init_f32((phaser_f32 *)fx->states[2], fx->states[1], p);

    // Assign processing callback for phaser effect
    fx->process = phaser_f32_process;
    fx->clean =   fx_phaser_clean;


}


float32_t* fx_phaser_parameters(FX_HANDLER_t *fx){

	phaser_params_t *p = (phaser_params_t*)fx->states[0];

	return p->params;

}

void fx_phaser_clean(FX_HANDLER_t *fx){

	phaser_f32_clean((phaser_f32 *)fx->states[2]);

    for(int i = 0; i < 8; i++)
    	fx->states[i] = NULL;

    fx->process = NULL;
}

/*
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
*/
