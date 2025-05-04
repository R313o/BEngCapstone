

#include "_MULTI_FX.h"


convolution_reverb_f32 convolution_reverb = {

    .fir1 = &fir_emt_140_dark_3,

	.process =&convolution_reverb_process
};

__attribute__((section(".dtcm"), aligned(32))) float state5[BUFFER_SIZE];


static void convolution_reverb_process(pipe *p){

	partitioned_fir_convolution_fft(p, convolution_reverb.fir1, state5);

}


