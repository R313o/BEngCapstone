#include "partitioned_fir_convolution_fft.h"

#include "pipe.h"
#include "fir_struct.h"

extern arm_rfft_fast_instance_f32 fft;

void ova_convolve(pipe *pipe, fir_t *fir);

