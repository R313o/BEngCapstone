//#include "supro_simulation.h"
#include "_MULTI_FX.h"

supro_simulation_f32 supro_sim = {

    //.fir1 = &fir_emt_140_dark_3,
    .fir1 = &fir_h1_gaincorrected,
	.fir2 = &fir_h2_gaincorrected,
	.fir3 = &fir_h2_gaincorrected,

	//.process =&supro_process
};


#include "h1_fir.h"
#include "h2_fir.h"
#include "h3_fir.h"


__attribute__((section(".dtcm"), aligned(32))) float state[BUFFER_SIZE];

__attribute__((section(".dtcm"), aligned(32))) float state2[BUFFER_SIZE];

__attribute__((section(".dtcm"), aligned(32))) float state3[BUFFER_SIZE];


/* Parameter vector indices */
#define SUPRO_NUM_A_VALS      40
#define SUPRO_P_PRE_GAIN_IDX  (SUPRO_NUM_A_VALS + 0)   /* preGainVal */
#define SUPRO_P_POST_GAIN_IDX (SUPRO_NUM_A_VALS + 1)   /* postGainVal */
#define SUPRO_P_BLEND_IDX     (SUPRO_NUM_A_VALS + 2)   /* blendVal */
#define SUPRO_P_BIAS_IDX      (SUPRO_NUM_A_VALS + 3)   /* biasVal */
#define SUPRO_P_G_BIAS_IDX    (SUPRO_NUM_A_VALS + 4)   /* gBias */
#define SUPRO_P_KP_IDX        (SUPRO_NUM_A_VALS + 5)   /* kPVal */
#define SUPRO_P_GP_IDX        (SUPRO_NUM_A_VALS + 6)   /* gPVal */
#define SUPRO_P_KN_IDX        (SUPRO_NUM_A_VALS + 7)   /* kNVal */
#define SUPRO_P_GN_IDX        (SUPRO_NUM_A_VALS + 8)   /* gNVal */
#define SUPRO_P_G_PRE_IDX     (SUPRO_NUM_A_VALS + 9)   /* gPre */
#define SUPRO_P_G_POST_IDX    (SUPRO_NUM_A_VALS + 10)  /* gPost */
#define SUPRO_NUM_PARAMS      (SUPRO_NUM_A_VALS + 11)

#define SUPRO_TAYLOR_COEFF_COUNT 53

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

#define NUM_TAPS              512
#define BLOCK_SIZE 			  BUFFER_SIZE


#define LP_STAGES 1
__attribute__((section(".dtcm"), aligned(32)))
static float32_t preampLP_State[LP_STAGES*4];
__attribute__((section(".dtcm"), aligned(32)))
static float32_t powerampLP_State[LP_STAGES*4];

static arm_biquad_casd_df1_inst_f32 preampLP;
static arm_biquad_casd_df1_inst_f32 powerampLP;


//arm_fir_instance_f32 h1_fir_f32;
//arm_fir_instance_f32 h2_fir_f32;
//arm_fir_instance_f32 h3_fir_f32;

//static float32_t h1_fir[BLOCK_SIZE + h1_fir_LEN - 1];
//static float32_t h2_fir[BLOCK_SIZE + h2_fir_LEN - 1];
//static float32_t h3_fir[BLOCK_SIZE + h3_fir_LEN - 1];


static const float32_t LP5Hz_Biquad[5] =
{
    1.07042518514e-07f,
    2.14085037028e-07f,
    1.07042518514e-07f,
    1.99907439945f,     /* -(-1.9990744) */
   -0.99907482762f      /* -( 0.99907483) */
};

void supro_simulation_init_f32(supro_simulation_f32 *self, fir_t *fir1, fir_t *fir2, fir_t*fir3) //float32_t *state
{

    self->fir1  = fir1;
    self->fir2  = fir2;
    self->fir3  = fir3;
   // self->state = state;


    arm_biquad_cascade_df1_init_f32(&preampLP,  LP_STAGES,
                                    (float32_t *)LP5Hz_Biquad,
                                    preampLP_State);
    arm_biquad_cascade_df1_init_f32(&powerampLP, LP_STAGES,
                                    (float32_t *)LP5Hz_Biquad,
                                    powerampLP_State);

	//arm_fir_init_f32(&h1_fir_f32, h1_fir_LEN, (float32_t *)&h1_fir_coeffs[0], &h1_fir[0], (uint32_t)BLOCK_SIZE);
	//arm_fir_init_f32(&h2_fir_f32, h2_fir_LEN, (float32_t *)&h2_fir_coeffs[0], &h2_fir[0], (uint32_t)BLOCK_SIZE);
	//arm_fir_init_f32(&h3_fir_f32, h3_fir_LEN, (float32_t *)&h3_fir_coeffs[0], &h3_fir[0], (uint32_t)BLOCK_SIZE);


}


/* --- implementation --- */

void supro_simulation_f32_process(supro_simulation_f32 *self, pipe *p)
{

	arm_scale_f32(p->processBuffer, 0.1, p->processBuffer, BUFFER_SIZE);

    /* 1) First FIR filter */
    // ...
	//partitioned_fir_convolution_fft(p, supro_sim.fir1, state, fftOut, zeropad);
	partitioned_fir_convolution_fft(p, supro_sim.fir1, state);
    //arm_fir_f32(&h1_fir_f32, p->processBuffer, p->processBuffer, BLOCK_SIZE);


    /* Preamp shaper */
	supro_preamp_f32(p);


    /* Second FIR filter */
	//partitioned_fir_convolution_fft(p, supro_sim.fir2, state2, fftOut2, zeropad2);
    //arm_fir_f32(&h2_fir_f32, p->processBuffer, p->processBuffer, BLOCK_SIZE);
	partitioned_fir_convolution_fft(p, supro_sim.fir2, state2);


	/* Poweramp shaper */
    supro_poweramp_f32(p);

	/* Third FIR filter */
	//partitioned_fir_convolution_fft(p, supro_sim.fir3, state3, fftOut3, zeropad3);
	partitioned_fir_convolution_fft(p, supro_sim.fir3, state3);
    //arm_fir_f32(&h3_fir_f32, p->processBuffer, p->processBuffer, BLOCK_SIZE);


	//arm_scale_f32(p->processBuffer, 0.0001, p->processBuffer, BUFFER_SIZE);
	arm_scale_f32(p->processBuffer, 0.01, p->processBuffer, BUFFER_SIZE);


}

static float32_t pre_scratch[BUFFER_SIZE];
static float32_t pre_env    [BUFFER_SIZE];

void supro_preamp_f32(pipe *p)
{
    float32_t *x     = p->processBuffer;
    float32_t *scratch = pre_scratch;  // holds x^2, then xpre+xenvBias, then final mix
    float32_t *env     = pre_env;      // holds filtered envelope and yMap

    const float32_t *a     = supro_parameters;
    const float32_t  gPre  = supro_parameters[SUPRO_P_PRE_GAIN_IDX];
    const float32_t  gPost = supro_parameters[SUPRO_P_POST_GAIN_IDX];
    const float32_t  gWet  = supro_parameters[SUPRO_P_BLEND_IDX];
    const float32_t  bias  = supro_parameters[SUPRO_P_BIAS_IDX];

    /* 1) Compute squared signal into scratch */
    arm_mult_f32(x, x, scratch, BUFFER_SIZE);

    /* 2) Low‑pass envelope: scratch→env */
    arm_biquad_cascade_df1_f32(&preampLP, scratch, env, BUFFER_SIZE);

    /* 3) envelope = sqrt(2 * env) (in env) */
    arm_scale_f32(env, 2.0f, env, BUFFER_SIZE);
    for (uint32_t i = 0; i < BUFFER_SIZE; ++i) {
        float32_t v = env[i];
        env[i] = sqrtf(v > 0.0f ? v : 0.0f);
    }

    /* 4) pre‑gain path: xpre = gPre * x  (in scratch) */
    arm_scale_f32(x, gPre, scratch, BUFFER_SIZE);

    /* 5) add envelope bias: scratch += bias * env */
    arm_scale_f32(env, bias, env, BUFFER_SIZE);
    arm_add_f32(scratch, env, scratch, BUFFER_SIZE);

    /* 6) Horner polynomial: yMap into env */
    uint32_t K = SUPRO_NUM_A_VALS;
    arm_scale_f32(scratch, a[K-1], env, BUFFER_SIZE);
    for (int k = K-2; k >= 0; --k) {
        arm_mult_f32(scratch, env, env, BUFFER_SIZE);
        for (uint32_t i = 0; i < BUFFER_SIZE; ++i) {
            env[i] += a[k];
        }
    }

    /* 7) wet/dry mix + post‑gain → final output (reuse scratch) */
    arm_scale_f32(env, gWet,     env,     BUFFER_SIZE);
    arm_scale_f32(scratch, 1-gWet, scratch, BUFFER_SIZE);
    arm_add_f32(env, scratch, scratch, BUFFER_SIZE);
    arm_scale_f32(scratch, gPost, x,       BUFFER_SIZE);
}



static float32_t pow_scratch[BUFFER_SIZE];

void supro_poweramp_f32(pipe *p)
{
    const float32_t gPre   = supro_parameters[SUPRO_P_G_PRE_IDX];
    const float32_t gPost  = supro_parameters[SUPRO_P_G_POST_IDX];
    const float32_t gBias  = supro_parameters[SUPRO_P_G_BIAS_IDX];
    const float32_t kN     = supro_parameters[SUPRO_P_KN_IDX];
    const float32_t kP     = supro_parameters[SUPRO_P_KP_IDX];
    const float32_t gN     = supro_parameters[SUPRO_P_GN_IDX];
    const float32_t gP     = supro_parameters[SUPRO_P_GP_IDX];

    float32_t *scratch = pow_scratch;
    float32_t *x       = p->processBuffer;

    // 1) scratch = x^2
    arm_mult_f32(x, x, scratch, BUFFER_SIZE);

    // 2) scratch = LP(scratch)
    arm_biquad_cascade_df1_f32(&powerampLP, scratch, scratch, BUFFER_SIZE);

    // 3) scratch = sqrt(2 * scratch)
    arm_scale_f32(scratch, 2.0f, scratch, BUFFER_SIZE);
    for (uint32_t i = 0; i < BUFFER_SIZE; ++i) {
        float32_t v = scratch[i];
        scratch[i]  = sqrtf(v > 0.0f ? v : 0.0f);
    }

    // 4) precompute tanh constants
    float32_t tanh_kN = tanhf( kN );
    float32_t tanh_kP = tanhf( kP );
    float32_t coeffN  = (tanh_kN*tanh_kN - 1.0f) / gN;
    float32_t coeffP  = (tanh_kP*tanh_kP - 1.0f) / gP;

    // 5) per‑sample non‑linear mapping + post‑gain
    for (uint32_t i = 0; i < BUFFER_SIZE; ++i) {
        float32_t env   = scratch[i];
        float32_t xBias = x[i] - gBias * env;
        float32_t xPre  = gPre  * xBias;
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

