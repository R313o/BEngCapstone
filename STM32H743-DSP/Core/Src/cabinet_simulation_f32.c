
//#include "supro_simulation.h"
#include "_MULTI_FX.h"


/*
cabinet_simulation_f32 cabinet_sim = {

    .fir1 = &fir_OD_M212_VINT_DYN_201_P05_00,

	.process =&cabinet_process
};
*/

__attribute__((section(".dtcm"), aligned(32))) float state4[BUFFER_SIZE];


void cabinet_simulation_f32_process(cabinet_simulation_f32 *self, pipe *p){

	partitioned_fir_convolution_fft(p, self->fir1, self->state);

}
void cabinet_simulation_f32_init(cabinet_simulation_f32 *self, float32_t *state, fir_t *fir){


	self->fir1 = fir;
	self->state = state;

}



