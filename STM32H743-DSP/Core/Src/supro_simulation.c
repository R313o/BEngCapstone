#include "supro_simulation.h"


supro_simulation_f32 supro_sim = {

    //.fir1 = &fir_emt_140_dark_3,
    .fir1 = &fir_h1_gaincorrected,
	.fir2 = &fir_h2_gaincorrected,
	.fir3 = &fir_h2_gaincorrected,

	.process =&supro_process
};

cabinet_simulation_f32 cabinet_sim = {

    .fir1 = &fir_OD_M212_VINT_DYN_201_P05_00,

	.process =&cabinet_process
};



#include "h1_fir.h"
#include "h2_fir.h"
#include "h3_fir.h"


__attribute__((section(".dtcm"), aligned(32))) float zeropad[FFT_SIZE];
__attribute__((section(".dtcm"), aligned(32))) float state[BUFFER_SIZE];
float fftOut[FFT_SIZE];

__attribute__((section(".dtcm"), aligned(32))) float zeropad2[FFT_SIZE];
__attribute__((section(".dtcm"), aligned(32))) float state2[BUFFER_SIZE];
float fftOut2[FFT_SIZE];

__attribute__((section(".dtcm"), aligned(32))) float zeropad3[FFT_SIZE];
__attribute__((section(".dtcm"), aligned(32))) float state3[BUFFER_SIZE];
float fftOut3[FFT_SIZE];

__attribute__((section(".dtcm"), aligned(32))) float zeropad4[FFT_SIZE];
__attribute__((section(".dtcm"), aligned(32))) float state4[BUFFER_SIZE];
float fftOut4[FFT_SIZE];


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

//arm_fir_instance_f32 preamp_fir_5Hz_lowpass;
//arm_fir_instance_f32 poweramp_fir_5Hz_lowpass;


//static float32_t preamp_firState_f32[BLOCK_SIZE + NUM_TAPS - 1];
//static float32_t poweramp_firState_f32[BLOCK_SIZE + NUM_TAPS - 1];

#define LP_STAGES 1                    /* one biquad ⇒ 2‑pole */
__attribute__((section(".dtcm"), aligned(32)))
static float32_t preampLP_State[LP_STAGES*4];   /* 4 words per stage (DF1) */

__attribute__((section(".dtcm"), aligned(32)))
static float32_t powerampLP_State[LP_STAGES*4];

static arm_biquad_casd_df1_inst_f32 preampLP;
static arm_biquad_casd_df1_inst_f32 powerampLP;


arm_fir_instance_f32 h1_fir_f32;
arm_fir_instance_f32 h2_fir_f32;
arm_fir_instance_f32 h3_fir_f32;

static float32_t h1_fir[BLOCK_SIZE + h1_fir_LEN - 1];
static float32_t h2_fir[BLOCK_SIZE + h2_fir_LEN - 1];
static float32_t h3_fir[BLOCK_SIZE + h3_fir_LEN - 1];


static const float32_t LP5Hz_Biquad[5] =
{
    1.07042518514e-07f,
    2.14085037028e-07f,
    1.07042518514e-07f,
    1.99907439945f,     /* -(-1.9990744) */
   -0.99907482762f      /* -( 0.99907483) */
};

void supro_init_f32()
{

	//arm_fir_init_f32(&preamp_fir_5Hz_lowpass, NUM_TAPS, (float32_t *)&firCoeffs32[0], &preamp_firState_f32[0], (uint32_t)BLOCK_SIZE);
	//arm_fir_init_f32(&poweramp_fir_5Hz_lowpass, NUM_TAPS, (float32_t *)&firCoeffs32[0], &poweramp_firState_f32[0], (uint32_t)BLOCK_SIZE);
    arm_biquad_cascade_df1_init_f32(&preampLP,  LP_STAGES,
                                    (float32_t *)LP5Hz_Biquad,
                                    preampLP_State);
    arm_biquad_cascade_df1_init_f32(&powerampLP, LP_STAGES,
                                    (float32_t *)LP5Hz_Biquad,
                                    powerampLP_State);

	arm_fir_init_f32(&h1_fir_f32, h1_fir_LEN, (float32_t *)&h1_fir_coeffs[0], &h1_fir[0], (uint32_t)BLOCK_SIZE);
	arm_fir_init_f32(&h2_fir_f32, h2_fir_LEN, (float32_t *)&h2_fir_coeffs[0], &h2_fir[0], (uint32_t)BLOCK_SIZE);
	arm_fir_init_f32(&h3_fir_f32, h3_fir_LEN, (float32_t *)&h3_fir_coeffs[0], &h3_fir[0], (uint32_t)BLOCK_SIZE);

}


/* --- implementation --- */

void supro_process(pipe *p)
{

	arm_scale_f32(p->processBuffer, 0.1, p->processBuffer, BUFFER_SIZE);

    /* 1) First FIR filter */
    // ...
	partitioned_fir_convolution_fft(p, supro_sim.fir1, state, fftOut, zeropad);
    //arm_fir_f32(&h1_fir_f32, p->processBuffer, p->processBuffer, BLOCK_SIZE);


    /* Preamp shaper */
	supro_preamp_f32(p);


    /* Second FIR filter */
	partitioned_fir_convolution_fft(p, supro_sim.fir2, state2, fftOut2, zeropad2);
    //arm_fir_f32(&h2_fir_f32, p->processBuffer, p->processBuffer, BLOCK_SIZE);


	/* Poweramp shaper */
    supro_poweramp_f32(p);

	/* Third FIR filter */
	partitioned_fir_convolution_fft(p, supro_sim.fir3, state3, fftOut3, zeropad3);
    //arm_fir_f32(&h3_fir_f32, p->processBuffer, p->processBuffer, BLOCK_SIZE);


	//arm_scale_f32(p->processBuffer, 0.0001, p->processBuffer, BUFFER_SIZE);
	arm_scale_f32(p->processBuffer, 0.01, p->processBuffer, BUFFER_SIZE);


}

void supro_preamp_f32(pipe *p)
{
	//arm_scale_f32(p->processBuffer, 0.1, p->processBuffer, BLOCK_SIZE); // temporary scaling until SPENCER corrects pipe scaling

    float32_t temp[BLOCK_SIZE], envelope[BLOCK_SIZE], xpre[BLOCK_SIZE];
    float32_t xmapped[BLOCK_SIZE], yMap[BLOCK_SIZE], yDry[BLOCK_SIZE];
    const float32_t
        *a     = &supro_parameters[0],                    // Taylor coefs
        *gPre  = &supro_parameters[SUPRO_P_PRE_GAIN_IDX],    // pre-gain
        *gPost = &supro_parameters[SUPRO_P_POST_GAIN_IDX],   // post-gain (p[N+11])
        *gWet  = &supro_parameters[SUPRO_P_BLEND_IDX],    // wet mix
        *bias  = &supro_parameters[SUPRO_P_BIAS_IDX];     // envelope bias (p[N+4])

    /* 1) envelope = sqrt( 2 * LP( x^2 ) ) */
    arm_mult_f32(p->processBuffer, p->processBuffer, temp, BLOCK_SIZE);
    //arm_fir_f32(&preamp_fir_5Hz_lowpass, temp, envelope, BLOCK_SIZE);
    arm_biquad_cascade_df1_f32(&preampLP, temp, envelope, BLOCK_SIZE);
    arm_scale_f32(envelope, 2.0f, temp, BLOCK_SIZE);

    for(uint32_t i = 0; i < (uint32_t)BLOCK_SIZE; i++){
        float32_t v = temp[i];
        envelope[i] = sqrtf(v > 0.0f ? v : 0.0f);
    }

    /* 2) pre-gain */
    arm_scale_f32(p->processBuffer, *gPre, xpre, BLOCK_SIZE);

    /* 3) add envelope bias */
    arm_scale_f32(envelope, *bias, temp, BLOCK_SIZE);
    arm_add_f32(xpre, temp, xmapped, BLOCK_SIZE);

    /* 4) Horner polynomial on xmapped */
    arm_scale_f32(xmapped,  a[SUPRO_NUM_A_VALS-1], yMap, BLOCK_SIZE);
    for (int k = SUPRO_NUM_A_VALS-2; k >= 0; --k) {
        arm_mult_f32(xmapped, yMap, yMap, BLOCK_SIZE);
        for (uint32_t i = 0; i < BLOCK_SIZE; ++i)
            yMap[i] += a[k];
    }

    /* 5) wet/dry mix */
    arm_scale_f32(yMap, *gWet, temp, BLOCK_SIZE);
    arm_scale_f32(xpre, 1.0f - *gWet, yDry, BLOCK_SIZE);
    arm_add_f32(temp, yDry, temp, BLOCK_SIZE);

    /* 6) post-gain */
    arm_scale_f32(temp, *gPost, p->processBuffer, BLOCK_SIZE);


}


void supro_poweramp_f32(pipe *p)
{
	//arm_scale_f32(p->processBuffer, 0.1, p->processBuffer, BLOCK_SIZE); // temporary scaling until SPENCER corrects pipe scaling

    float32_t temp[BLOCK_SIZE], envelope[BLOCK_SIZE];

    /* 1) envelope = sqrt( 2 * LP( x^2 ) ) */
    arm_mult_f32(p->processBuffer, p->processBuffer, temp, BLOCK_SIZE);
    //arm_fir_f32(&poweramp_fir_5Hz_lowpass, temp, envelope, BLOCK_SIZE);
    arm_biquad_cascade_df1_f32(&powerampLP, temp, envelope, BLOCK_SIZE);
    arm_scale_f32(envelope, 2.0f, temp, BLOCK_SIZE);

    for(uint32_t i = 0; i < (uint32_t)BLOCK_SIZE; i++){
        float32_t v = temp[i];
        envelope[i] = sqrtf(v > 0.0f ? v : 0.0f);
    }

	const float32_t *gPre   = &supro_parameters[SUPRO_P_G_PRE_IDX];
	const float32_t *gPost  = &supro_parameters[SUPRO_P_G_POST_IDX];
	const float32_t *gBias  = &supro_parameters[SUPRO_P_G_BIAS_IDX];

	const float32_t *kN     = &supro_parameters[SUPRO_P_KN_IDX];
	const float32_t *kP     = &supro_parameters[SUPRO_P_KP_IDX];
	const float32_t *gN     = &supro_parameters[SUPRO_P_GN_IDX];
	const float32_t *gP     = &supro_parameters[SUPRO_P_GP_IDX];

	/* Pre-compute constants used in every sample */
	const float32_t kN_val   = *kN,         kP_val   = *kP;
	const float32_t gN_val   = *gN,         gP_val   = *gP;
	const float32_t tanh_kN  = tanhf(kN_val);
	const float32_t tanh_kP  = tanhf(kP_val);
	const float32_t coeffN   = (tanh_kN*tanh_kN - 1.0f) / gN_val;
	const float32_t coeffP   = (tanh_kP*tanh_kP - 1.0f) / gP_val;

	/* ──────────────────────────────────────────
	 * 3. Per-sample processing loop
	 * ────────────────────────────────────────── */
	for (uint32_t i = 0; i < BLOCK_SIZE; ++i)
	{
		/* 3.1 Feed-forward bias & pre-gain (MATLAB: xBias, xPre) */
		float32_t xBias = p->processBuffer[i] - (*gBias) * envelope[i];
		float32_t xPre  = (*gPre) * xBias;              /* dry path copy */
		float32_t m;                                    /* shaped sample */

		/* 3.2 Piece-wise tanh mapping (Eq. 6) */
		if (xPre > kP_val) {                    /* Region A */
			m = tanh_kP - coeffP * tanhf(gP_val * xPre - kP_val);
		} else if (xPre >= -kN_val) {           /* Region B */
			m = tanhf(xPre);
		} else {                                /* Region C */
			m = -tanh_kN - coeffN * tanhf(gN_val * xPre + kN_val);
		}

		/* 3.3 Wet/dry blend then post-gain */
		//m = (*gWet) * m + (1.0f - *gWet) * xPre;   /* yMap in MATLAB   */
		p->processBuffer[i] = (*gPost) * m;        /* y output         */
	}


}


void cabinet_process(pipe *p){

	partitioned_fir_convolution_fft(p, cabinet_sim.fir1, state4, fftOut4, zeropad4);

}
