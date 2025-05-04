

#include "_MULTI_FX.h"


void convolution_reverb_f32_process(convolution_reverb_f32 *self, pipe *p){

	partitioned_fir_convolution_fft(p, convolution_reverb.fir1, self->state);

}


void convolution_reverb_f32_init(convolution_reverb_f32 *self, float32_t *state){

	self->state = state;
	self->fir1 = &fir_emt_140_dark_3;


}


