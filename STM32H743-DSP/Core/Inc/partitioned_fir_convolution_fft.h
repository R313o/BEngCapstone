/**
  ******************************************************************************
  * @file    partitioned_fir_convolution_fft.h
  * @brief   Extern declarations and API for the partitioned FFT-based convolution engine.
  *
  * This header exposes:
  *   - The CMSIS-DSP real FFT instance (arm_rfft_fast_instance_f32 fft) for
  *     single-precision real-to-complex transforms.
  *   - The partitioned_fir_convolution_fft() function to perform one block of
  *     overlap–add FFT convolution.
  *
  * Include this file to initialize your FFT instance and invoke the fast‐convolution
  * processing routines within your audio pipeline.
  *
  * @author
  *   Hassan Islam
  * @date
  *   April 2025
  ******************************************************************************
  */

#ifndef FAST_CONVOLUTION_H
#define FAST_CONVOLUTION_H

#include "pipe.h"
#include "impulse_responses.h"

/**
  * @brief  CMSIS-DSP real FFT instance for single-precision transforms.
  *         Configured via arm_rfft_fast_init_f32() before use in convolution.
  */
extern arm_rfft_fast_instance_f32 fft;

/**
 * @brief   Perform one block of overlap–add FFT convolution.
 * @param   pipe     Audio I/O and buffer context
 * @param   fir      FIR handler (contains IR FFTs, etc)
 * @param   state    Overlap buffer (previous FFT output)
 * @param   fftOut   Scratch buffer for this block’s FFT output
 * @param   zeropad  Input buffer zero-padded to FFT size
 */
/*
void partitioned_fir_convolution_fft( pipe *pipe,
                                      fir_t  *fir,
                                      float  *state,
                                      float  *fftOut,
                                      float  *zeropad );
                                      */

void partitioned_fir_convolution_fft( pipe *pipe,
                                      fir_t  *fir,
                                      float  *state );
#endif
