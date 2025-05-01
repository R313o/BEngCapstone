#include "supro_simulation.h"


supro_simulation_f32 supro_sim = {

    //.fir1 = &fir_emt_140_dark_3,
    .fir1 = &fir_h1_gaincorrected,
	.fir2 = &fir_h2_gaincorrected,
	.fir3 = &fir_h2_gaincorrected,

	.process =&supro_process
};

__attribute__((section(".dtcm"), aligned(32))) float zeropad[FFT_SIZE];
__attribute__((section(".dtcm"), aligned(32))) float state[BUFFER_SIZE];
float fftOut[FFT_SIZE];

__attribute__((section(".dtcm"), aligned(32))) float zeropad2[FFT_SIZE];
__attribute__((section(".dtcm"), aligned(32))) float state2[BUFFER_SIZE];
float fftOut2[FFT_SIZE];

__attribute__((section(".dtcm"), aligned(32))) float zeropad3[FFT_SIZE];
__attribute__((section(".dtcm"), aligned(32))) float state3[BUFFER_SIZE];
float fftOut3[FFT_SIZE];


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

arm_fir_instance_f32 preamp_fir_5Hz_lowpass;
arm_fir_instance_f32 poweramp_fir_5Hz_lowpass;

static float32_t preamp_firState_f32[BLOCK_SIZE + NUM_TAPS - 1];
static float32_t poweramp_firState_f32[BLOCK_SIZE + NUM_TAPS - 1];

static const float firCoeffs32[NUM_TAPS] = {
     0.00001659f,  0.00001660f,  0.00001662f,  0.00001666f,  0.00001671f,  0.00001678f,  0.00001686f,  0.00001696f,  0.00001708f,  0.00001721f,
     0.00001736f,  0.00001751f,  0.00001767f,  0.00001784f,  0.00001802f,  0.00001820f,  0.00001839f,  0.00001858f,  0.00001878f,  0.00001898f,
     0.00001919f,  0.00001940f,  0.00001962f,  0.00001984f,  0.00002006f,  0.00002028f,  0.00002051f,  0.00002074f,  0.00002097f,  0.00002121f,
     0.00002144f,  0.00002168f,  0.00002192f,  0.00002216f,  0.00002240f,  0.00002264f,  0.00002288f,  0.00002312f,  0.00002336f,  0.00002360f,
     0.00002384f,  0.00002408f,  0.00002432f,  0.00002456f,  0.00002481f,  0.00002505f,  0.00002530f,  0.00002554f,  0.00002579f,  0.00002603f,
     0.00002628f,  0.00002652f,  0.00002677f,  0.00002701f,  0.00002726f,  0.00002750f,  0.00002775f,  0.00002799f,  0.00002824f,  0.00002848f,
     0.00002873f,  0.00002898f,  0.00002922f,  0.00002947f,  0.00002972f,  0.00002996f,  0.00003021f,  0.00003046f,  0.00003071f,  0.00003095f,
     0.00003120f,  0.00003145f,  0.00003169f,  0.00003194f,  0.00003219f,  0.00003244f,  0.00003269f,  0.00003293f,  0.00003318f,  0.00003343f,
     0.00003367f,  0.00003392f,  0.00003416f,  0.00003441f,  0.00003465f,  0.00003490f,  0.00003514f,  0.00003539f,  0.00003563f,  0.00003588f,
     0.00003612f,  0.00003637f,  0.00003661f,  0.00003686f,  0.00003710f,  0.00003735f,  0.00003759f,  0.00003784f,  0.00003808f,  0.00003833f,
     0.00003857f,  0.00003882f,  0.00003906f,  0.00003931f,  0.00003955f,  0.00003980f,  0.00004004f,  0.00004029f,  0.00004053f,  0.00004078f,
     0.00004102f,  0.00004127f,  0.00004151f,  0.00004176f,  0.00004200f,  0.00004225f,  0.00004249f,  0.00004274f,  0.00004298f,  0.00004323f,
     0.00004347f,  0.00004372f,  0.00004396f,  0.00004421f,  0.00004445f,  0.00004470f,  0.00004494f,  0.00004519f,  0.00004543f,  0.00004568f,
     0.00004592f,  0.00004617f,  0.00004641f,  0.00004666f,  0.00004690f,  0.00004715f,  0.00004739f,  0.00004764f,  0.00004788f,  0.00004813f,
     0.00004837f,  0.00004862f,  0.00004886f,  0.00004911f,  0.00004935f,  0.00004960f,  0.00004984f,  0.00005009f,  0.00005033f,  0.00005058f,
     0.00005082f,  0.00005107f,  0.00005131f,  0.00005156f,  0.00005180f,  0.00005205f,  0.00005229f,  0.00005254f,  0.00005278f,  0.00005303f,
     0.00005327f,  0.00005352f,  0.00005376f,  0.00005401f,  0.00005425f,  0.00005450f,  0.00005474f,  0.00005499f,  0.00005523f,  0.00005548f,
     0.00005572f,  0.00005597f,  0.00005621f,  0.00005646f,  0.00005670f,  0.00005695f,  0.00005719f,  0.00005744f,  0.00005768f,  0.00005793f,
     0.00005817f,  0.00005842f,  0.00005866f,  0.00005891f,  0.00005915f,  0.00005940f,  0.00005964f,  0.00005989f,  0.00006013f,  0.00006038f,
     0.00006062f,  0.00006087f,  0.00006111f,  0.00006136f,  0.00006160f,  0.00006185f,  0.00006209f,  0.00006234f,  0.00006258f,  0.00006283f,
     0.00006307f,  0.00006332f,  0.00006356f,  0.00006381f,  0.00006405f,  0.00006430f,  0.00006454f,  0.00006479f,  0.00006503f,  0.00006528f,
     0.00006552f,  0.00006577f,  0.00006601f,  0.00006626f,  0.00006650f,  0.00006675f,  0.00006699f,  0.00006724f,  0.00006748f,  0.00006773f,
     0.00006797f,  0.00006822f,  0.00006846f,  0.00006871f,  0.00006895f,  0.00006920f,  0.00006944f,  0.00006969f,  0.00006993f,  0.00007018f,
     0.00007042f,  0.00007067f,  0.00007091f,  0.00007116f,  0.00007140f,  0.00007165f,  0.00007189f,  0.00007214f,  0.00007238f,  0.00007263f,
     0.00007287f,  0.00007312f,  0.00007336f,  0.00007361f,  0.00007385f,  0.00007410f,  0.00007434f,  0.00007459f,  0.00007483f,  0.00007508f,
     0.00007532f,  0.00007557f,  0.00007581f,  0.00007606f,  0.00007630f,  0.00007655f,  0.00007679f,  0.00007704f,  0.00007728f,  0.00007753f,
     0.00007777f,  0.00007802f,  0.00007826f,  0.00007851f,  0.00007875f,  0.00007900f,  0.00007924f,  0.00007949f,  0.00007973f,  0.00007998f,
     0.00008022f,  0.00008047f,  0.00008071f,  0.00008096f,  0.00008120f,  0.00008145f,  0.00008169f,  0.00008194f,  0.00008218f,  0.00008243f,
     0.00008267f,  0.00008292f,  0.00008316f,  0.00008341f,  0.00008365f,  0.00008390f,  0.00008414f,  0.00008439f,  0.00008463f,  0.00008488f,
     0.00008512f,  0.00008537f,  0.00008561f,  0.00008586f,  0.00008610f,  0.00008635f,  0.00008659f,  0.00008684f,  0.00008708f,  0.00008733f,
     0.00008757f,  0.00008782f,  0.00008806f,  0.00008831f,  0.00008855f,  0.00008880f,  0.00008904f,  0.00008929f,  0.00008953f,  0.00008978f,
};




void supro_init_f32()
{

	arm_fir_init_f32(&preamp_fir_5Hz_lowpass, NUM_TAPS, (float32_t *)&firCoeffs32[0], &preamp_firState_f32[0], (uint32_t)BLOCK_SIZE);
	arm_fir_init_f32(&poweramp_fir_5Hz_lowpass, NUM_TAPS, (float32_t *)&firCoeffs32[0], &poweramp_firState_f32[0], (uint32_t)BLOCK_SIZE);

}


/* --- implementation --- */

void supro_process(pipe *p)
{

	arm_scale_f32(p->processBuffer, 0.1, p->processBuffer, BUFFER_SIZE);
    /* 1) First FIR filter */
    // ...
	partitioned_fir_convolution_fft(p, supro_sim.fir1, state, fftOut, zeropad);


    /* Preamp shaper */
	supro_preamp_f32(p);


    /* Second FIR filter */
	partitioned_fir_convolution_fft(p, supro_sim.fir2, state2, fftOut2, zeropad2);

	/* Poweramp shaper */


	/* Third FIR filter */
	partitioned_fir_convolution_fft(p, supro_sim.fir3, state3, fftOut3, zeropad3);


	arm_scale_f32(p->processBuffer, 0.00002, p->processBuffer, BUFFER_SIZE);

}

void supro_preamp_f32(pipe *p)
{
	//arm_scale_f32(p->processBuffer, 0.1, p->processBuffer, BLOCK_SIZE); // temporary scaling until SPENCER corrects pipe scaling

    float32_t temp[BLOCK_SIZE], envelope[BLOCK_SIZE], xpre[BLOCK_SIZE];
    float32_t xmapped[BLOCK_SIZE], yMap[BLOCK_SIZE], yDry[BLOCK_SIZE];
    const float32_t
        *a     = &supro_parameters[0],                    // Taylor coefs
        *gPre  = &supro_parameters[SUPRO_P_G_PRE_IDX],    // pre-gain
        *gPost = &supro_parameters[SUPRO_P_G_POST_IDX],   // post-gain (p[N+11])
        *gWet  = &supro_parameters[SUPRO_P_BLEND_IDX],    // wet mix
        *bias  = &supro_parameters[SUPRO_P_BIAS_IDX];     // envelope bias (p[N+4])

    /* 1) envelope = sqrt( 2 * LP( x^2 ) ) */
    arm_mult_f32(p->processBuffer, p->processBuffer, temp, BLOCK_SIZE);
    arm_fir_f32(&preamp_fir_5Hz_lowpass, temp, envelope, BLOCK_SIZE);
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
    arm_fir_f32(&poweramp_fir_5Hz_lowpass, temp, envelope, BLOCK_SIZE);
    arm_scale_f32(envelope, 2.0f, temp, BLOCK_SIZE);

    for(uint32_t i = 0; i < (uint32_t)BLOCK_SIZE; i++){
        float32_t v = temp[i];
        envelope[i] = sqrtf(v > 0.0f ? v : 0.0f);
    }

	const float32_t *gPre   = &supro_parameters[SUPRO_P_G_PRE_IDX];
	const float32_t *gPost  = &supro_parameters[SUPRO_P_G_POST_IDX];
	const float32_t *gWet   = &supro_parameters[SUPRO_P_BLEND_IDX];
	const float32_t *gBias  = &supro_parameters[SUPRO_P_BIAS_IDX];

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
		m = (*gWet) * m + (1.0f - *gWet) * xPre;   /* yMap in MATLAB   */
		p->processBuffer[i] = (*gPost) * m;        /* y output         */
	}


}

