#include "supro_simulation.h"
#include "emt_140_dark_3.h"      // defines fir_emt_140_dark_3

supro_simulation_f32 supro_sim = {
    .fir1 = &fir_emt_140_dark_3,
	.process =&supro_process
};

/* --- implementation --- */
void supro_process(pipe *p)
{
    /* 1) First FIR filter */
    // ...
	ova_convolve(p, supro_sim.fir1);

    /* 2) Preamp shaper */
    // ...

    /* 3) Second FIR filter */
    // ...
}
