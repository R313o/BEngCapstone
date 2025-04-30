#ifndef H2_GAINCORRECTED_IR_H
#define H2_GAINCORRECTED_IR_H

#include "fir_struct.h"

extern __attribute__((aligned(32))) const float _H2_IR_FFT_ALL[2048];

extern __attribute__((aligned(32))) float _H2_prev_ffts[2048];

extern fir_t fir_h2_gaincorrected;

#endif
