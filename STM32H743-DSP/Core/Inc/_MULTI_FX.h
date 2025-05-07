/**
 * @file    _MULTI_FX.h
 * @brief   FX processer main kernel
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
    //FX_CHORUS,    /**< Chorus effect */
    FX_CABINET,   /**< Cabinet simulation */
    FX_SUPRO,      /**< Supro simulation */
	FX_NULL        /**< no effect*/
} MULTI_FX_type_t;

/**
 * @brief Generic FX handler structure
 */
typedef struct FX_HANDLER_t {
    MULTI_FX_type_t      type;      /**< FX type identifier */
    void (*process)(struct FX_HANDLER_t *self, pipe *p); /**< Processing callback */
    void (*clean)(struct FX_HANDLER_t *self);
    void *states[8];                 /**< Pointers to effect-specific state buffers */
} FX_HANDLER_t;


/**
 * @brief Initialize a reverb FX handler
 * @param fx Pointer to an FX_HANDLER_t to initialize
 */
void fx_reverb_init(FX_HANDLER_t *fx);
void fx_reverb_clean(FX_HANDLER_t *fx);
/**
 * @brief Initialize a cabinet FX handler
 * @param fx Pointer to an FX_HANDLER_t to initialize
 */
void fx_cabinet_init(FX_HANDLER_t *fx);
void fx_cabinet_clean(FX_HANDLER_t *fx);


/**
 * @brief Initialize a Supro FX handler
 * @param fx Pointer to an FX_HANDLER_t to initialize
 */
void fx_supro_init(FX_HANDLER_t *fx);
void fx_supro_clean(FX_HANDLER_t *fx);


void fx_null_init(FX_HANDLER_t *fx);

/*
 * @brief Effects Look up table
 */
extern void (*fx_init[])(FX_HANDLER_t *fx );

/**
 * @brief Global FX handler instances (user-defined)
 */
extern FX_HANDLER_t fx_handle_0;
extern FX_HANDLER_t fx_handle_1;
extern FX_HANDLER_t fx_handle_2;


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




#endif // SUPRO_SIMULATION_H_
