#include "supro_simulation.h"
#include "emt_140_dark_3.h"      // defines fir_emt_140_dark_3
<<<<<<< Updated upstream
#include "fastConvolution.h"     // defines ova_convolve
=======
#include "h1_gaincorrected.h"
#include "h2_gaincorrected.h"
#include "h3_gaincorrected.h"
>>>>>>> Stashed changes


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


/* --- implementation --- */
void supro_process(pipe *p)
{
    /* 1) First FIR filter */
    // ...
	//ova_convolve(p, supro_sim.fir1);
	//ova_convolve(p, supro_sim.fir2);

	ova_convolve(p, supro_sim.fir1, state, fftOut, zeropad);

	ova_convolve(p, supro_sim.fir2, state2, fftOut2, zeropad2);

	ova_convolve(p, supro_sim.fir3, state3, fftOut3, zeropad3);

	arm_scale_f32(p->processBuffer, 1, p->processBuffer, BUFFER_SIZE);

    /* 2) Preamp shaper */
    // ...

    /* 3) Second FIR filter */
    // ...
}
