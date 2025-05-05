

#include "_MULTI_FX.h"

/*
void convolution_reverb_f32_process(FX_HANDLER_t *self, pipe *p){

	partitioned_fir_convolution_fft(p, self->fir1, self->state);
}
*/


void convolution_reverb_f32_init(convolution_reverb_f32 *self, float32_t *state, fir_t *fir){

	self->state = state;
	self->fir1 = fir;
}


void convolution_reverb_f32_process(FX_HANDLER_t *self, pipe *p){

	partitioned_fir_convolution_fft(p, self->states[2], self->states[1]);

}
//void convolution_reverb_f32_init(FX_HANDLER_t *self, float32_t *state, fir_t *fir);


#define NUMSEGMENTS_EMT 46U
#define NUMSEGMENTS_CAB 1U

void fx_reverb_init(FX_HANDLER_t *fx ) {   /*initialization and mem allocation for a reverb*/

	  fx->states[0] = (float*)_static_mem_alloc(

		(NUMSEGMENTS_EMT*FFT_SIZE + 2*NUMSEGMENTS_EMT)*sizeof(float),

		_Alignof(float)

		);

	  fx->states[1] = (float *)_dctm_static_mem_alloc(

		BUFFER_SIZE*sizeof(float),

		_Alignof(float)

		);

	  fx->states[2] = (fir_t*)_static_mem_alloc(

		 sizeof(fir_t),

		_Alignof(fir_t)

		);

	  fx->states[3] = (convolution_reverb_f32*)_static_mem_alloc(

		 sizeof(convolution_reverb_f32),

		_Alignof(convolution_reverb_f32)

		);

	  fir_emt_140_dark_3_f32_init(fx->states[2], fx->states[0]);

	  convolution_reverb_f32_init(fx->states[3], fx->states[1], fx->states[2]);

	  fx->process = convolution_reverb_f32_process;

  }
