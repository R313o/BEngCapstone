/**
  ******************************************************************************
  * @file    supro_simulation.h
  * @brief   Multi-stage FFT-based convolution simulation interface.
  *
  * This header exposes:
  *   - The supro_simulation_f32 struct, grouping three FIR filters and a
  *     processing callback for the audio pipeline.
  *   - The global simulation instance `supro_sim`.
  *   - The `supro_process()` prototype, which dispatches each filter stage.
  *   - Three sets of zero-pad, overlap (state), and FFT output buffers,
  *     all 32-byte aligned in DTCM for DMA/SIMD-optimized FFT routines.
  *
  * Include this file to configure and run the three-stage convolution chain.
  *
  * @author
  *   Hassan Islam
  * @date
  *   April 2025
  ******************************************************************************
  */

#ifndef INC_SUPRO_SIMULATION_H_
#define INC_SUPRO_SIMULATION_H_

#include "pipe.h"                      			// pipe_t
#include "partitioned_fir_convolution_fft.h"  	// ova_convolve API
#include "impulse_responses.h"         			// IR FFT tables & FIR handlers

/**
  * @brief  Simulation context for three parallel FIR stages.
  */
typedef struct {
    fir_t *fir1;     	 /**< Handler for stage-1 filter  */
    fir_t *fir2;      	 /**< Handler for stage-2 filter  */
    fir_t *fir3;      	 /**< Handler for stage-3 filter  */
    void  (*process)(pipe *p);  /**< Processing callback */
} supro_simulation_f32;



/******************************************************************/
/* Global instance of the supro simulation.                       */
/******************************************************************/
extern supro_simulation_f32 supro_sim;


/**
  * @brief  Top-level processing function for the three-stage convolution chain.
  * @param  p Pointer to the audio pipe context
  */
void supro_process(pipe *p);


void supro_preamp_f32();
void supro_poweramp_f32();

extern void supro_init_f32();

/*********************************************************************/
/* FIR buffers: zero-pad input, overlap buffer, FFT output (float32) */
/*********************************************************************/

/*  Stage 1 buffers */
extern __attribute__((section(".dtcm"), aligned(32))) float zeropad[FFT_SIZE];
extern __attribute__((section(".dtcm"), aligned(32))) float state[BUFFER_SIZE];
extern float fftOut[FFT_SIZE];

/* Stage 2 buffers */
extern __attribute__((section(".dtcm"), aligned(32))) float zeropad2[FFT_SIZE];
extern __attribute__((section(".dtcm"), aligned(32))) float state2[BUFFER_SIZE];
extern float fftOut2[FFT_SIZE];

/* Stage 3 fir buffers*/
extern __attribute__((section(".dtcm"), aligned(32))) float zeropad3[FFT_SIZE];
extern __attribute__((section(".dtcm"), aligned(32))) float state3[BUFFER_SIZE];
extern float fftOut3[FFT_SIZE];




#endif /* INC_SUPRO_SIMULATION_H_ */
