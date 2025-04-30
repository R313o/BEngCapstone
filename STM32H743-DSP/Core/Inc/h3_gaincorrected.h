#ifndef H3_GAINCORRECTED_IR_H
#define H3_GAINCORRECTED_IR_H

#include "fir_struct.h"

extern __attribute__((aligned(32))) const float _H3_IR_FFT_ALL[2048];

extern __attribute__((aligned(32))) float _H3_prev_ffts[2048];

extern fir_t fir_h3_gaincorrected;

#endif
