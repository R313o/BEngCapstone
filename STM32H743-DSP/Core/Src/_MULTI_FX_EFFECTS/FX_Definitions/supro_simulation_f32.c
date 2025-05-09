/**
 * @file    supro_simulation.c
 * @brief   Multi-stage Supro FX chain: convolution reverb, preamp shaping, and poweramp shaping.
 *
 * This implementation allocates and initializes:
 *   - Three partitioned-FIR convolution engines (reverb stages)
 *   - Biquad lowpass filters for envelope detection in preamp and poweramp
 *   - Internal state buffers in DTCM and normal RAM
 *
 * Author: Hassan Islam
 * Date:   May 2025
 */

#include "_MULTI_FX.h"
#include "partitioned_fir_convolution_fft.h"
#include "arm_math.h"

#define _static_mem_alloc _static_mem_alloc_ram_d2

/**
 * @brief internal initialization
 */
static void supro_simulation_init_f32(supro_simulation_f32 *self, float32_t *state,
							  fir_t *fir1,
							  fir_t *fir2,
							  fir_t *fir3);

static void supro_simulation_clean_f32(supro_simulation_f32 *self);

/**
 * @brief Internal Supro processing step
 * @param p Pointer to the audio pipe context
 */
static void supro_simulation_f32_process(FX_HANDLER_t *self, pipe *p);

/**
 * @brief Internal Supro preamp stage
 */
static void supro_preamp_f32(FX_HANDLER_t *self, pipe *p);

/**
 * @brief Internal Supro poweramp stage
 */
static void supro_poweramp_f32(FX_HANDLER_t *self, pipe *p);


//==============================================================================
// Parameter Definitions
//==============================================================================
#define LP_STAGES             1
#define SUPRO_NUM_A_VALS      40
#define SUPRO_P_PRE_GAIN_IDX  (SUPRO_NUM_A_VALS + 0)
#define SUPRO_P_POST_GAIN_IDX (SUPRO_NUM_A_VALS + 1)
#define SUPRO_P_BLEND_IDX     (SUPRO_NUM_A_VALS + 2)
#define SUPRO_P_BIAS_IDX      (SUPRO_NUM_A_VALS + 3)
#define SUPRO_P_G_BIAS_IDX    (SUPRO_NUM_A_VALS + 4)
#define SUPRO_P_KP_IDX        (SUPRO_NUM_A_VALS + 5)
#define SUPRO_P_GP_IDX        (SUPRO_NUM_A_VALS + 6)
#define SUPRO_P_KN_IDX        (SUPRO_NUM_A_VALS + 7)
#define SUPRO_P_GN_IDX        (SUPRO_NUM_A_VALS + 8)
#define SUPRO_P_G_PRE_IDX     (SUPRO_NUM_A_VALS + 9)
#define SUPRO_P_G_POST_IDX    (SUPRO_NUM_A_VALS + 10)
#define SUPRO_TAYLOR_COEFF_COUNT 53


// Taylor coefficients + gains + mix values
static const float supro_parameters[SUPRO_TAYLOR_COEFF_COUNT] = {
    2.9959557839e+00f, 1.6314218341e+02f, 5.2905971278e+02f, -2.5420406389e+04f, -8.7626301838e+04f,
    1.4459545520e+06f, 4.1880734277e+06f, -4.2442516258e+07f, -1.0187779617e+08f, 7.5399425176e+08f,
    1.5095263453e+09f, -8.8780365066e+09f, -1.4940208767e+10f, 7.3417926974e+10f, 1.0445722785e+11f,
    -4.4340510596e+11f, -5.3514241325e+11f, 2.0092704677e+12f, 2.0585676719e+12f, -6.9588684224e+12f,
    -6.0413079225e+12f, 1.8641231340e+13f, 1.3650519452e+13f, -3.8869616773e+13f, -2.3823897058e+13f,
    6.3160691904e+13f, 3.2026363055e+13f, -7.9633734117e+13f, -3.2840742490e+13f, 7.7058324601e+13f,
    2.5207550227e+13f, -5.6105511909e+13f, -1.4015588326e+13f, 2.9721907314e+13f, 5.3291084293e+12f,
    -1.0808499657e+13f, -1.2392685164e+12f, 2.4118903600e+12f, 1.3290912318e+11f, -2.4893286587e+11f,
    9.4155141253e-01f, 9.9109658491e-01f, 5.5227207795e-01f, 5.9523293851e-01f, 6.2042179680e-01f,
    9.9207059793e-01f, 1.0032398973e+00f, 9.9783294635e-01f, 9.9854033522e-01f, 1.1138744286e+00f,
    6.7112551269e+00f, 1.1138744377e+00f, 6.7112551294e+00f
};


// 5 Hz lowpass biquad coefficients (b0, b1, b2, -a1, -a2)
static const float32_t LP5Hz_Biquad[5] = {
    1.07042518514e-07f,
    2.14085037028e-07f,
    1.07042518514e-07f,
    1.99907439945f,
   -0.99907482762f
};


float32_t* fx_supro_parameters(FX_HANDLER_t *fx){

	return NULL;
}

//==============================================================================
// Public FX Initialization
//==============================================================================
/**
 * @brief Configure an FX_HANDLER for the Supro chain.
 */
void fx_supro_init(FX_HANDLER_t *fx)
{

	fx->num_params = 0;

    // Allocate overlap state buffers in DTCM: 3 × BUFFER_SIZE
    fx->states[0] = _dctm_static_mem_alloc(
        3 * BUFFER_SIZE * sizeof(float32_t), _Alignof(float32_t)
    );

    // Compute offset for each convolver block: scratch + 2×segments
    #define H1_SCRATCH_FLOATS  (H1_SEGMENTS * FFT_SIZE)
    #define BUFFER_OFFSET      (H1_SCRATCH_FLOATS + 2 * H1_SEGMENTS)

    // Allocate FFT-domain buffers for all 3 FIR engines
    fx->states[1] = _static_mem_alloc(
        3 * BUFFER_OFFSET * sizeof(float32_t), _Alignof(float32_t)
    );

    // Allocate array of 3 fir_t structs
    fx->states[2] = _static_mem_alloc(
        3 * sizeof(fir_t), _Alignof(fir_t)
    );

    // return if mem allocation fails
    for(int i = 0 ; i < 3 ; ++i){
    	if(fx->states[i] == NULL){
    		fx_supro_clean(fx);
    	      return;
    	}
    }

    // Initialize each FIR with its slice of the FFT buffer
    {
        float32_t *fftBuf = (float32_t *)fx->states[1];
        fir_t     *firs   = (fir_t *)fx->states[2];
        fir_h1_f32_init(&firs[0], &fftBuf[0 * BUFFER_OFFSET]);
        fir_h2_f32_init(&firs[1], &fftBuf[1 * BUFFER_OFFSET]);
        fir_h3_f32_init(&firs[2], &fftBuf[2 * BUFFER_OFFSET]);
    }

    // Allocate biquad filter states in DTCM
    fx->states[3] = _dctm_static_mem_alloc(LP_STAGES * 4 * sizeof(float32_t), _Alignof(float32_t));
    fx->states[4] = _dctm_static_mem_alloc(LP_STAGES * 4 * sizeof(float32_t), _Alignof(float32_t));

    // Allocate biquad instances in normal RAM
    fx->states[5] = _static_mem_alloc(sizeof(arm_biquad_casd_df1_inst_f32), _Alignof(arm_biquad_casd_df1_inst_f32));
    fx->states[6] = _static_mem_alloc(sizeof(arm_biquad_casd_df1_inst_f32), _Alignof(arm_biquad_casd_df1_inst_f32));

    // return if mem allocation fails
    for(int i = 3 ; i < 7 ; ++i){
    	if(fx->states[i] == NULL){
    		fx_supro_clean(fx);
    	      return;
    	}
    }


    // Initialize filters
    arm_biquad_cascade_df1_init_f32((arm_biquad_casd_df1_inst_f32 *)fx->states[5],
                                   LP_STAGES, (float32_t *)LP5Hz_Biquad,
                                   (float32_t *)fx->states[3]);
    arm_biquad_cascade_df1_init_f32((arm_biquad_casd_df1_inst_f32 *)fx->states[6],
                                   LP_STAGES, (float32_t *)LP5Hz_Biquad,
                                   (float32_t *)fx->states[4]);

    // Allocate and initialize the internal simulation object
    fx->states[7] = _static_mem_alloc(sizeof(supro_simulation_f32), _Alignof(supro_simulation_f32));
    {
        float32_t *dtcmState = (float32_t *)fx->states[0];
        fir_t     *firs      = (fir_t *)fx->states[2];
        supro_simulation_init_f32((supro_simulation_f32 *)fx->states[7],
                                  dtcmState, &firs[0], &firs[1], &firs[2]);
    }


	if(fx->states[7] == NULL){
		fx_supro_clean(fx);
		  return;
	}


    fx->process = supro_simulation_f32_process;
    fx->clean = fx_reverb_clean;
}


void fx_supro_clean(FX_HANDLER_t *fx)
{

	fir_t     *firs   = (fir_t *)fx->states[2];

	fir_h1_f32_init_clean((fir_t *)&firs[0]);
	fir_h2_f32_init_clean((fir_t *)&firs[1]);
	fir_h3_f32_init_clean((fir_t *)&firs[2]);

	supro_simulation_clean_f32((supro_simulation_f32*)fx->states[7]);

    for(int i = 0; i < 8; i++)
    	fx->states[i] = NULL;

    fx->process = NULL;

}

//==============================================================================
// Implementation Details
//==============================================================================
/**
 * @brief Initialize supro_simulation struct with FIR handlers and DTCM state.
 */
static void supro_simulation_init_f32(supro_simulation_f32 *self,
                                      float32_t *state,
                                      fir_t *fir1,
                                      fir_t *fir2,
                                      fir_t *fir3)
{
    self->fir1  = fir1;
    self->fir2  = fir2;
    self->fir3  = fir3;
    self->state = state;
}


static void supro_simulation_clean_f32(supro_simulation_f32 *self)
{
    self->fir1  = NULL;
    self->fir2  = NULL;
    self->fir3  = NULL;
    self->state = NULL;
}



/**
 * @brief Main FX chain processing: reverb → preamp → reverb → poweramp → reverb.
 */
static void supro_simulation_f32_process(FX_HANDLER_t *fx, pipe *p)
{
    float32_t *overlap = (float32_t *)fx->states[0];
    fir_t     *firs    = (fir_t *)fx->states[2];

    // 1) Dry signal attenuation
    arm_scale_f32(p->processBuffer, 0.07f, p->processBuffer, BUFFER_SIZE);

    // 2) First convolution reverb
    partitioned_fir_convolution_fft(p, &firs[0], &overlap[0]);

    // 3) Preamp shaping
    supro_preamp_f32(fx, p);

    // 4) Second convolution reverb
    partitioned_fir_convolution_fft(p, &firs[1], &overlap[BUFFER_SIZE]);

    // 5) Poweramp shaping
    supro_poweramp_f32(fx, p);

    // 6) Third convolution reverb
    partitioned_fir_convolution_fft(p, &firs[2], &overlap[2 * BUFFER_SIZE]);

    // 7) Final output scaling
    arm_scale_f32(p->processBuffer, 0.005f * 1/15, p->processBuffer, BUFFER_SIZE);

}

/**
 * @brief Preamp stage: envelope detection with lowpass → polynomial distortion → mix.
 */
static void supro_preamp_f32(FX_HANDLER_t *fx, pipe *p)
{
    static float32_t pre_scratch[BUFFER_SIZE];
    static float32_t pre_env[BUFFER_SIZE];
    float32_t        *x       = p->processBuffer;
    float32_t        *scratch = pre_scratch;
    float32_t        *env     = pre_env;

    const float32_t *a      = supro_parameters;
    float32_t        gPre   = a[SUPRO_P_PRE_GAIN_IDX];
    float32_t        gPost  = a[SUPRO_P_POST_GAIN_IDX];
    float32_t        gWet   = a[SUPRO_P_BLEND_IDX];
    float32_t        bias   = a[SUPRO_P_BIAS_IDX];

    // Envelope detection: squared → lowpass → sqrt(2·env)
    arm_mult_f32(x, x, scratch, BUFFER_SIZE);
    arm_biquad_cascade_df1_f32((arm_biquad_casd_df1_inst_f32 *)fx->states[5], scratch, env, BUFFER_SIZE);
    arm_scale_f32(env, 2.0f, env, BUFFER_SIZE);
    for (uint32_t i = 0; i < BUFFER_SIZE; ++i) env[i] = sqrtf(fmaxf(env[i], 0.0f));

    // Polynomial distortion via Horner's method
    arm_scale_f32(x, gPre, scratch, BUFFER_SIZE);
    arm_scale_f32(env, bias, env, BUFFER_SIZE);
    arm_add_f32(scratch, env, scratch, BUFFER_SIZE);
    uint32_t K = SUPRO_NUM_A_VALS;
    arm_scale_f32(scratch, a[K-1], env, BUFFER_SIZE);
    for (int k = K-2; k >= 0; --k) {
        arm_mult_f32(scratch, env, env, BUFFER_SIZE);
        for (uint32_t i = 0; i < BUFFER_SIZE; ++i) env[i] += a[k];
    }

    // Wet/dry mix and postgain
    arm_scale_f32(env, gWet, env, BUFFER_SIZE);
    arm_scale_f32(scratch, 1 - gWet, scratch, BUFFER_SIZE);
    arm_add_f32(env, scratch, scratch, BUFFER_SIZE);
    arm_scale_f32(scratch, gPost, x, BUFFER_SIZE);
}

/**
 * @brief Poweramp stage: envelope detection → tanh-based distortion → postgain.
 */
static void supro_poweramp_f32(FX_HANDLER_t *fx, pipe *p)
{
    static float32_t pow_scratch[BUFFER_SIZE];
    const float32_t *a     = supro_parameters;
    float32_t        gPre  = a[SUPRO_P_G_PRE_IDX];
    float32_t        gPost = a[SUPRO_P_G_POST_IDX];
    float32_t        gBias = a[SUPRO_P_BIAS_IDX];
    float32_t        kN    = a[SUPRO_P_KN_IDX];
    float32_t        kP    = a[SUPRO_P_KP_IDX];
    float32_t        gN    = a[SUPRO_P_GN_IDX];
    float32_t        gP    = a[SUPRO_P_GP_IDX];

    float32_t *x       = p->processBuffer;
    float32_t *scratch = pow_scratch;

    // Envelope detection
    arm_mult_f32(x, x, scratch, BUFFER_SIZE);
    arm_biquad_cascade_df1_f32((arm_biquad_casd_df1_inst_f32 *)fx->states[6], scratch, scratch, BUFFER_SIZE);
    arm_scale_f32(scratch, 2.0f, scratch, BUFFER_SIZE);
    for (uint32_t i = 0; i < BUFFER_SIZE; ++i) scratch[i] = sqrtf(fmaxf(scratch[i], 0.0f));

    // Precompute constants for tanh segments
    float32_t tanh_kN = tanhf(kN);
    float32_t tanh_kP = tanhf(kP);
    float32_t coeffN  = (tanh_kN*tanh_kN - 1.0f) / gN;
    float32_t coeffP  = (tanh_kP*tanh_kP - 1.0f) / gP;

    // Nonlinearity per sample
    for (uint32_t i = 0; i < BUFFER_SIZE; ++i) {
        float32_t env   = scratch[i];
        float32_t xBias = x[i] - gBias * env;
        float32_t xPre  = gPre * xBias;
        float32_t m;
        if (xPre > kP) {
            m = tanh_kP - coeffP * tanhf(gP * xPre - kP);
        } else if (xPre >= -kN) {
            m = tanhf(xPre);
        } else {
            m = -tanh_kN - coeffN * tanhf(gN * xPre + kN);
        }
        x[i] = gPost * m;
    }
}
