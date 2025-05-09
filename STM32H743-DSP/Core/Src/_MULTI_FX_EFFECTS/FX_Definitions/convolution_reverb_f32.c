/**
 * @file    fx_reverb.c
 * @brief   Convolution reverb effect initialization & processing for multi-FX engine.
 *
 * Allocates memory for FFT-domain buffers, overlap state, FIR filter handler,
 * and binds the processing callback for convolution reverb.
 *
 * @author
 *   Hassan Islam
 * @date
 *   May 2025
 */

#include "_MULTI_FX.h"
#include "stdio.h"


/**
 * @brief Initialize a convolution reverb instance
 * @param self  Pointer to convolution_reverb_f32 to initialize
 * @param state Pointer to user-provided state buffer
 * @param fir   Pointer to FIR filter handler
 */
static void convolution_reverb_f32_init(convolution_reverb_f32 *self,
                                 float32_t *state,
                                 fir_t *fir);

/**
 * @brief Convolution reverb processing callback
 * @param self Pointer to FX_HANDLER_t base
 * @param p    Pointer to the audio pipe context
 */
static void convolution_reverb_f32_process(FX_HANDLER_t *self, pipe *p);



//-----------------------------------------------------------------------------
// Convolution Reverb Instance Management
//-----------------------------------------------------------------------------

/**
 * @brief Initialize a convolution reverb instance.
 *
 * Sets up the state buffer and FIR filter handler on the convolution_reverb_f32 struct.
 *
 * @param self  Pointer to convolution_reverb_f32 to initialize.
 * @param state Pointer to DTCM-aligned overlap/state buffer (BUFFER_SIZE floats).
 * @param fir   Pointer to FIR filter handler instance.
 */
static void convolution_reverb_f32_init(
    convolution_reverb_f32 *self,
    float32_t *state,
    fir_t     *fir
)
{
    self->state = state;
    self->fir1  = fir;
}


/**
 * @brief Initialize a convolution reverb instance.
 *
 * Sets up the state buffer and FIR filter handler on the convolution_reverb_f32 struct.
 *
 * @param self  Pointer to convolution_reverb_f32 to initialize.
 * @param state Pointer to DTCM-aligned overlap/state buffer (BUFFER_SIZE floats).
 * @param fir   Pointer to FIR filter handler instance.
 */
void convolution_reverb_f32_clean(
    convolution_reverb_f32 *self
)
{
   self->state = NULL;
   self->fir1  = NULL;
}



/**
 * @brief Convolution reverb processing callback.
 *
 * Executes a single-stage partitioned FFT convolution for the reverb effect.
 *
 * @param self Generic FX handler containing allocated state and FIR pointers.
 * @param p    Audio pipeline context.
 */
static void convolution_reverb_f32_process(
    FX_HANDLER_t *self,
    pipe         *p
)
{
    // self->states[2]: FIR handler pointer
    // self->states[1]: overlap/state buffer pointer
    partitioned_fir_convolution_fft(
        p,
        (fir_t     *)self->states[2],
        (float32_t *)self->states[1]
    );

    arm_scale_f32(p->processBuffer, 1.0f/2, p->processBuffer, BUFFER_SIZE);
}



float32_t* fx_reverb_parameters(FX_HANDLER_t *fx){

	return NULL;
}

//-----------------------------------------------------------------------------
// FX Reverb Initialization
//-----------------------------------------------------------------------------

#define NUMSEGMENTS_EMT 46U   /**< Number of FFT segments for EMT reverb IR */
#define NUMSEGMENTS_CAB 1U    /**< Single segment for cabinet simulation IR */

/**
 * @brief Configure a generic FX handler for convolution reverb effect.
 *
 * Allocates FFT-domain memory, overlap buffers, FIR handler, and
 * convolution_reverb instance, then initializes and assigns the processing callback.
 *
 * @param fx Pointer to FX_HANDLER_t to configure for reverb effect.
 */
void fx_reverb_init(FX_HANDLER_t *fx)
{

	fx->num_params = 0;

    // Allocate FFT-domain memory for reverb: numSegments * FFT_SIZE + space for Handlers
    fx->states[0] = _static_mem_alloc(
        (NUMSEGMENTS_EMT * FFT_SIZE + 2 * NUMSEGMENTS_EMT) * sizeof(float),
        _Alignof(float)
    );

    // Allocate DTCM state buffer for overlap (BUFFER_SIZE floats)
    fx->states[1] = _dctm_static_mem_alloc(
        BUFFER_SIZE * sizeof(float),
        _Alignof(float)
    );

    // Allocate FIR filter handler
    fx->states[2] = _static_mem_alloc(
        sizeof(fir_t),
        _Alignof(fir_t)
    );

    // Allocate convolution reverb simulation instance
    fx->states[3] = _static_mem_alloc(
        sizeof(convolution_reverb_f32),
        _Alignof(convolution_reverb_f32)
    );


    // return if mem allocation fails
    for(int i = 0 ; i < 4 ; ++i){
    	if(fx->states[i] == NULL){
    		fx_reverb_clean(fx);
    	      return;
    	}
    }


    // Initialize FIR handler with FFT memory
    fir_emt_140_dark_3_f32_init(
        (fir_t *)fx->states[2],
        (float *)fx->states[0]
    );

    // Initialize reverb simulation with state buffer and FIR handler
    convolution_reverb_f32_init(
        (convolution_reverb_f32 *)fx->states[3],
        (float32_t *)fx->states[1],
        (fir_t *)fx->states[2]
    );

    // Set processing callback for convolution reverb
    fx->process = convolution_reverb_f32_process;
    fx->clean =   fx_cabinet_clean;
}


void fx_reverb_clean(FX_HANDLER_t *fx)
{

    fir_emt_140_dark_3_f32_clean((fir_t *)fx->states[2]);
    convolution_reverb_f32_clean((convolution_reverb_f32*)fx->states[3]);

    for(int i = 0; i < 8; i++)
    	fx->states[i] = NULL;

    fx->process = NULL;

}
