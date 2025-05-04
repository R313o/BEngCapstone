
//#include "supro_simulation.h"
#include "_MULTI_FX.h"


cabinet_simulation_f32 cabinet_sim = {

    .fir1 = &fir_OD_M212_VINT_DYN_201_P05_00,

	.process =&cabinet_process
};

__attribute__((section(".dtcm"), aligned(32))) float state4[BUFFER_SIZE];


static void cabinet_process(pipe *p){

	partitioned_fir_convolution_fft(p, cabinet_sim.fir1, state4);

}


