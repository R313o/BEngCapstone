/**
 * @file    fx_cabinet.c
 * @brief   Cabinet simulation implementation for multi-FX engine.
 *
 * Allocates and initializes the FIR filter, state buffers, and processing callbacks
 * for the cabinet effect using partitioned FFT convolution.
 *
 * @author
 *   Hassan Islam
 * @date
 *   May 2025
 */

#include "_MULTI_FX.h"

#define _static_mem_alloc _static_mem_alloc_ram_d2

/**
 * @brief Private Initializer for a cabinet simulation instance
 * @param self  Pointer to cabinet_simulation_f32 to initialize
 * @param state Pointer to user-provided state buffer
 * @param fir   Pointer to FIR filter handler
 */
static void cabinet_simulation_f32_init(cabinet_simulation_f32 *self,
                                 float32_t *state,
                                 fir_t *fir);

/**
 * @brief Private Cabinet processing callback
 * @param self Pointer to FX_HANDLER_t base
 * @param p    Pointer to the audio pipe context
 */
static void cabinet_simulation_f32_process(FX_HANDLER_t *self, pipe *p);


/**
 * @brief Private Cabinet effect processing function.
 *
 * Dispatches a single-stage partitioned FIR convolution for the cabinet simulation.
 *
 * @param self Generic FX handler containing allocated state and FIR pointers.
 * @param p    Audio pipeline context.
 */
static void cabinet_simulation_f32_process(FX_HANDLER_t *self, pipe *p)
{
    // self->states[2]: pointer to FIR filter handler
    // self->states[1]: pointer to overlap/state buffer
    partitioned_fir_convolution_fft(
        p,
        (fir_t *)self->states[2],
        (float32_t *)self->states[1]
    );
}

/**
 * @brief Initialize a cabinet simulation instance.
 *
 * Sets the FIR handler and state buffer on a cabinet_simulation_f32 struct.
 *
 * @param self  Pointer to cabinet_simulation_f32 to initialize.
 * @param state Pointer to DTCM-aligned overlap/state buffer (BUFFER_SIZE floats).
 * @param fir   Pointer to FIR filter handler.
 */
static void cabinet_simulation_f32_init(
    cabinet_simulation_f32 *self,
    float32_t *state,
    fir_t *fir
)
{
    self->fir1  = fir;
    self->state = state;
}

static void cabinet_simulation_clean_f32(cabinet_simulation_f32 *self)
{
    self->fir1  = NULL;
    self->state = NULL;
}


float32_t* fx_cabinet_parameters(FX_HANDLER_t *fx){

	return NULL;
}


/**
 * @brief Configure a generic FX handler for cabinet simulation.
 *
 * Allocates necessary memory for FFT-domain buffers, overlap buffers, FIR handler,
 * and the cabinet simulation instance. Then initializes each component and sets
 * the processing callback.
 *
 * @param fx Pointer to FX_HANDLER_t to configure for cabinet effect.
 */
void fx_cabinet_init(FX_HANDLER_t *fx)
{
    const uint32_t numSegments = 1U;

    fx->num_params = 0;

    // Allocate FFT-domain memory for FIR (numSegments * FFT_SIZE floats + overlap paddings)
    fx->states[0] = _static_mem_alloc(
        (numSegments * FFT_SIZE + 2 * numSegments) * sizeof(float),
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

    // Allocate cabinet simulation instance
    fx->states[3] = _static_mem_alloc(
        sizeof(cabinet_simulation_f32),
        _Alignof(cabinet_simulation_f32)
    );

    // Initialize FIR filter with FFT memory
    fir_OD_M212_VINT_DYN_201_P05_00_f32_init(
        (fir_t *)fx->states[2],
        (float *)fx->states[0]
    );

    // Initialize cabinet simulation with state buffer and FIR handler
    cabinet_simulation_f32_init(
        (cabinet_simulation_f32 *)fx->states[3],
        (float32_t *)fx->states[1],
        (fir_t *)fx->states[2]
    );

    // Assign processing callback for cabinet effect
    fx->process = cabinet_simulation_f32_process;
    fx->clean =   fx_cabinet_clean;
}


void fx_cabinet_clean(FX_HANDLER_t *fx)
{

	fir_OD_M212_VINT_DYN_201_P05_00_f32_clean((fir_t *)fx->states[2]);
    cabinet_simulation_clean_f32((cabinet_simulation_f32*)fx->states[3]);

    for(int i = 0; i < 8; i++)
    	fx->states[i] = NULL;

    fx->process = NULL;
    fx->clean = NULL;

}
