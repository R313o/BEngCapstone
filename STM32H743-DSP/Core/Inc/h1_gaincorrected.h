#ifndef H1_GAINCORRECTED_IR_H
#define H1_GAINCORRECTED_IR_H

#include "fir_struct.h"

extern __attribute__((aligned(32))) const float _H1_IR_FFT_ALL[2048];

extern __attribute__((aligned(32))) float _H1_prev_ffts[2048];

extern fir_t fir_h1_gaincorrected;

#endif
