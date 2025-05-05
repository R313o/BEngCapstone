/**
 * @file    supro_simulation.h
 * @brief   Multi-stage FFT-based convolution simulation interface.
 *
 * This header exposes:
 *   - The `FX_HANDLER_t` struct for generic FX handling.
 *   - The `supro_simulation_f32` struct for three-stage Supro convolution.
 *   - The `cabinet_simulation_f32` and `convolution_reverb_f32` structs.
 *   - Initialization and processing callbacks for each effect.
 *
 * @author
 *   Hassan Islam
 * @date
 *   April 2025
 */

#ifndef SUPRO_SIMULATION_H_
#define SUPRO_SIMULATION_H_

#include "partitioned_fir_convolution_fft.h"
#include "mem_manager_multi_fx.h"

//=============================================================================
// Generic FX Handler
//=============================================================================

/**
 * @brief Supported multi-FX types
 */
typedef enum {
    FX_REVERB,    /**< Reverb effect */
    FX_CHORUS,    /**< Chorus effect */
    FX_CABINET,   /**< Cabinet simulation */
    FX_SUPRO      /**< Supro simulation */
} MULTI_FX_type_t;

/**
 * @brief Generic FX handler structure
 */
typedef struct FX_HANDLER_t {
    MULTI_FX_type_t      type;      /**< FX type identifier */
    void (*process)(struct FX_HANDLER_t *self, pipe *p); /**< Processing callback */
    void *states[5];                 /**< Pointers to effect-specific state buffers */
} FX_HANDLER_t;

/**
 * @brief Initialize a reverb FX handler
 * @param fx Pointer to an FX_HANDLER_t to initialize
 */
void fx_reverb_init(FX_HANDLER_t *fx);

/**
 * @brief Initialize a cabinet FX handler
 * @param fx Pointer to an FX_HANDLER_t to initialize
 */
void fx_cabinet_init(FX_HANDLER_t *fx);

/**
 * @brief Global FX handler instances (user-defined)
 */
extern FX_HANDLER_t fx_handle_0;
extern FX_HANDLER_t fx_handle_1;


//________________________(PRIVATE) STATIC DEFINITIONS___________________________//



//=============================================================================
// Supro Simulation
//=============================================================================

/**
 * @brief Supro multi-stage convolution simulation
 *
 * Groups three FIR filters and a pipeline processing callback.
 */
typedef struct {
	FX_HANDLER_t base;
    fir_t  *fir1;      /**< Stage-1 FIR filter */
    fir_t  *fir2;      /**< Stage-2 FIR filter */
    fir_t  *fir3;      /**< Stage-3 FIR filter */
    float32_t    *state;   /**< Overlap/state buffer */
    //void   (*process)(pipe *p); /**< Pipeline processing callback */
} supro_simulation_f32;

/**
 * @brief Global instance of the Supro simulation
 */
//extern supro_simulation_f32 supro_sim;

/**
 * @brief Initialize the Supro simulation (must be called before use)
 */
void supro_simulation_init_f32(supro_simulation_f32 *self, float32_t *state, fir_t *fir1, fir_t *fir2, fir_t *fir3);
/**
 * @brief Internal Supro processing step
 * @param p Pointer to the audio pipe context
 */
void supro_simulation_f32_process(supro_simulation_f32 *self, pipe *p);

/**
 * @brief Internal Supro preamp stage
 */
void supro_preamp_f32(pipe *p);

/**
 * @brief Internal Supro poweramp stage
 */
void supro_poweramp_f32(pipe *p);

//=============================================================================
// Cabinet Simulation
//=============================================================================

/**
 * @brief Cabinet simulation structure
 */
typedef struct {
    FX_HANDLER_t  base;    /**< Base FX handler */
    fir_t        *fir1;    /**< Cabinet FIR filter */
    float32_t    *state;   /**< Overlap/state buffer */
} cabinet_simulation_f32;

/**
 * @brief Initialize a cabinet simulation instance
 * @param self  Pointer to cabinet_simulation_f32 to initialize
 * @param state Pointer to user-provided state buffer
 * @param fir   Pointer to FIR filter handler
 */
static void cabinet_simulation_f32_init(cabinet_simulation_f32 *self,
                                 float32_t *state,
                                 fir_t *fir);

/**
 * @brief Cabinet processing callback
 * @param self Pointer to FX_HANDLER_t base
 * @param p    Pointer to the audio pipe context
 */
static void cabinet_simulation_f32_process(FX_HANDLER_t *self, pipe *p);

// extern cabinet_simulation_f32 cabinet_sim; // Optional global instance

//=============================================================================
// Convolution Reverb
//=============================================================================

/**
 * @brief Convolution reverb effect structure
 */
typedef struct {
    FX_HANDLER_t  base;    /**< Base FX handler */
    fir_t        *fir1;    /**< Reverb FIR filter */
    float32_t    *state;   /**< Overlap/state buffer */
} convolution_reverb_f32;

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



#endif // SUPRO_SIMULATION_H_
