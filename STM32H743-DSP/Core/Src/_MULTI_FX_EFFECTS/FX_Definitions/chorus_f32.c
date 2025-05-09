/*
 * chorus.c
 *
 *  Created on: Feb 27, 2025
 *      Author: spencer
 */

#include  "_MULTI_FX.h"

#define _static_mem_alloc _static_mem_alloc_ram_d2

static void chorus_f32_process(FX_HANDLER_t *fx, pipe *p){

	/* this computation is repeated in another function. Consider cleaning up in the future */

	chorus_f32 *c = fx->states[2];

	float32_t G_ff =  0.8;
	float32_t G_fb  = 0.5;

	c->params->wetness = c->params->params[0];
	c->params->depth = c->params->params[1];
	c->params->rate = c->params->params[2];

    uint32_t size1 = BUFFER_SIZE * 5;
    uint32_t size2 = (uint32_t)(ceilf((float32_t)c->params->baseDelay * (1.0f + c->params->depth)) * 2);

    uint32_t bufferSize = (size1 > size2) ? size1 : size2;

    if (bufferSize < BUFFER_SIZE) {
    	bufferSize = BUFFER_SIZE;
    }

	float32_t *inDelayed = fx->states[1];
	float32_t *outDelayed = &inDelayed[bufferSize];


    // Process each sample in the frame (assumed to be BUFFER_SIZE samples)
    for (uint32_t n = 0; n < BUFFER_SIZE; n++) {
        float32_t input = p->processBuffer[n];

        // Compute current delay (in samples) with modulation via an LFO.
        float32_t mod = sinf(2.0f * M_PI * c->params->rate * c->t);
        uint32_t currentDelay = (uint32_t)(roundf(c->params->baseDelay * (1.0f + c->params->depth * mod)));

        // Compute the read pointer in the circular delay buffer.
        uint32_t readPtr;
        if (currentDelay >= bufferSize) {
            readPtr = c->writePtr;  // Clamp if delay is too high
        } else {
            readPtr = (c->writePtr + bufferSize - currentDelay) % bufferSize;
        }

        // Compute output sample using a mix of dry and delayed signals.
        float32_t delayedInput = inDelayed[readPtr];
        float32_t delayedOutput = outDelayed[readPtr];
        float32_t output = (1.0f - c->params->wetness) * input + c->params->wetness * (G_fb * delayedOutput + (G_ff - G_fb) * delayedInput);

        // Update delay buffers at the current write pointer.
        inDelayed[c->writePtr] = input;
        outDelayed[c->writePtr] = output;

        // Write the processed sample back into the processBuffer.
        p->processBuffer[n] = output;

        // Advance the circular buffer pointer.
        c->writePtr = (c->writePtr + 1) % bufferSize;
        // Advance time by dt.
        c->t += c->dt;
    }

}


float32_t* fx_chorus_parameters(FX_HANDLER_t *fx){

	chorus_params_t *c = (chorus_params_t*)fx->states[0];

	return c->params;

}

static void chorus_init_f32(chorus_f32 *self, float32_t *state, chorus_params_t *p) {

    self->state = state;
    self->params = p;

    //self->params->num_params = 3;

    self->t = 0.0f;
    self->dt = 1.0f / 48000.0f ; // sample rate !magic # for now.
    self->writePtr = 0;
}

void chorus_f32_clean(chorus_f32 *self){

    self->state = NULL;
    self->params = NULL;

}


void fx_chorus_init(FX_HANDLER_t *fx){


	fx->num_params = 3; // 3 params for a chorus effect

    // allocate parameter instance
    fx->states[0] = _dctm_static_mem_alloc(
        sizeof(chorus_params_t),
        _Alignof(chorus_params_t)
    );

  	if(fx->states[0] == NULL){
  		fx_chorus_clean(fx);
    	    return;
   	}
    chorus_params_t *c = (chorus_params_t*)fx->states[0];

    /*
     * defaults
     */
    c->wetness     = 0.8  ;
	c->depth 	   = 0.01  ;
	c->rate        = 2.0  ;
	c->baseDelay   = 1000 ;

    // Compute a suitable delay buffer size:
    uint32_t size1 = BUFFER_SIZE * 5;
    uint32_t size2 = (uint32_t)(ceilf((float32_t)c->baseDelay * (1.0f + c->depth)) * 2);

    uint32_t bufferSize = (size1 > size2) ? size1 : size2;

    if (bufferSize < BUFFER_SIZE) {
    	bufferSize = BUFFER_SIZE;
    }

    // Allocate state buffers
	fx->states[1] = _static_mem_alloc(
        2 * (bufferSize) * sizeof(float),   // (bottom half is input, top half is output)
        _Alignof(float)
    );

    // allocate chorus instance
    fx->states[2] = _dctm_static_mem_alloc(
        sizeof(chorus_f32),
        _Alignof(chorus_f32)
    );

    // return if mem allocation fails
    for(int i = 0 ; i < 3 ; ++i){
    	if(fx->states[i] == NULL){
    		fx_chorus_clean(fx);
    	      return;
    	}
    }

    // Initialize chorus effect
    chorus_init_f32((chorus_f32* )fx->states[2], fx->states[1], c);

    // Assign processing callback for chorus effect
    fx->process = chorus_f32_process;
    fx->clean  = fx_chorus_clean;

}

void fx_chorus_clean(FX_HANDLER_t *fx){

	chorus_f32_clean((chorus_f32 *)fx->states[2]);

    for(int i = 0; i < 8; i++)
    	fx->states[i] = NULL;

    fx->process = NULL;
}

