#pragma GCC optimize ("Ofast,unroll-loops,fast-math")

#include "pipe.h"
#include "fir_struct.h"

extern arm_rfft_fast_instance_f32 fft;

void ova_convolve(pipe *pipe, fir_t *fir, float* state, float* fftOut, float* zeropad);


