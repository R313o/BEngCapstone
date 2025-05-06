/**
  ******************************************************************************
  * @file    impulse_responses.h
  * @brief   Extern declarations for FFT-domain impulse responses, overlap buffers,
  *          and FIR filter objects used by the fast-convolution engine.
  *
  * This header exposes:
  *   - Aligned arrays (_IR_FFT_ALL) containing precomputed FFTs of each impulse response.
  *   - Aligned state buffers (prev_ffts_…_) holding the last FFT frame for overlap–save.
  *   - fir_t handler instances for each IR, configured for use in partitioned convolution.
  *
  * All arrays are 32-byte aligned to facilitate DMA transfers and SIMD-optimized FFT routines.
  * Include this file wherever you need to instantiate or reset convolution filters.
  *
  * @author
  *   Hassan Islam
  * @date
  *   April 2025
  ******************************************************************************
  */

#ifndef INC_IMPULSE_RESPONSES_H_
#define INC_IMPULSE_RESPONSES_H_

#define FFT_SIZE       2048

#include <stdint.h>

typedef struct {
    const float** ir_ffts;
    float**       prev_ffts;
    int32_t       curr_fftidx;
    int32_t       prev_fftidx;
    int32_t       numSegments;
} fir_t;


/******************************************************************/
/* Ampifier preamp-H1 impulse response                            */
/******************************************************************/
extern __attribute__((aligned(32))) const float _H1_IR_FFT_ALL[2048];

#define H1_SEGMENTS   1U
#define H1_SCRATCH_FLOATS (H1_SEGMENTS * FFT_SIZE)      /* 2048 */

void fir_h1_f32_init(fir_t *self, float *state);


/******************************************************************/
/* Ampifier stage-2-H2 correction impulse response                */
/******************************************************************/
extern __attribute__((aligned(32))) const float _H2_IR_FFT_ALL[2048];

#define H2_SEGMENTS   1U
#define H2_SCRATCH_FLOATS (H2_SEGMENTS * FFT_SIZE)      /* 2048 */

void fir_h2_f32_init(fir_t *self, float *state);


/******************************************************************/
/* Ampifier power_amplifer-H3 impulse response                    */
/******************************************************************/
extern __attribute__((aligned(32))) const float _H3_IR_FFT_ALL[2048];

#define H3_SEGMENTS   1U
#define H3_SCRATCH_FLOATS (H3_SEGMENTS * FFT_SIZE)      /* 2048 */

void fir_h3_f32_init(fir_t *self, float *state);


/******************************************************************/
/* EMT_140_DARK_3 impulse response                                */
/******************************************************************/
extern __attribute__((aligned(32))) const float _EMT_IR_FFT_ALL[94208];


#define EMT_SEGMENTS   46U
#define EMT_SCRATCH_FLOATS (EMT_SEGMENTS * FFT_SIZE)      /* 94 208 */

void fir_emt_140_dark_3_f32_init(fir_t *self, float *state);

/******************************************************************/
/* OD-M212-VINT-DYN-201-P05-00 impulse response                   */
/******************************************************************/
extern __attribute__((aligned(32))) const float _CAB_IR_FFT_ALL[2048];

#define OD_M212_SEGMENTS   1U
#define OD_M212_SCRATCH_FLOATS (OD_M212_SEGMENTS * FFT_SIZE)      /* 2048 */

void fir_OD_M212_VINT_DYN_201_P05_00_f32_init(fir_t *self, float *state);


#endif /* INC_IMPULSE_RESPONSES_H_ */
