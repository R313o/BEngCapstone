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

#ifndef INC_MULTI_FX_H_
#define INC_MULTI_FX_H_

#include "partitioned_fir_convolution_fft.h"

/******************************************************************/
/* supro simulation struct.				                          */
/******************************************************************/
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
 * @brief supro simulation iniitialization function. Call once before using process
 */
extern void supro_init_f32();


/**
  * @brief  Top-level private processing functions for the supro simulation
  * @param  p Pointer to the audio pipe context
  */
static void supro_process(pipe *p);
static void supro_preamp_f32();
static void supro_poweramp_f32();


/******************************************************************/
/* Cabinet simulation struct.				                      */
/******************************************************************/
typedef struct {
    fir_t *fir1;     	 /**< Handler for stage-1 filter  */
    void  (*process)(pipe *p);  /**< Processing callback */
} cabinet_simulation_f32;


/**
  * @brief  Top-level private processing functions for the cabinet simulation
  * @param  p Pointer to the audio pipe context
  */
static void cabinet_process(pipe *p);


/******************************************************************/
/* Global instance of the cabinet simulation.                     */
/******************************************************************/
extern cabinet_simulation_f32 cabinet_sim;



/******************************************************************/
/* Convolution reverb effect struct.				              */
/******************************************************************/
typedef struct {
    fir_t *fir1;     	 		/**< Handler for stage-1 filter  */
    void  (*process)(pipe *p);  /**< Processing callback */
} convolution_reverb_f32;


/**
  * @brief  Top-level private processing functions for the convolution reverb
  * @param  p Pointer to the audio pipe context
  */
static void convolution_reverb_process(pipe *p);

/******************************************************************/
/* Global instance of the convolution reverb                   */
/******************************************************************/
extern convolution_reverb_f32 convolution_reverb;




#endif /* INC_MULTI_FX_H_ */
