#ifndef INC_SUPRO_SIMULATION_H_
#define INC_SUPRO_SIMULATION_H_

#include "fir_struct.h"   // for fir_t
#include "pipe.h"         // for pipe

typedef struct supro_simulation_f32 {
    fir_t *fir1, *fir2, *fir3;
    void  (*process)(pipe *p);
} supro_simulation_f32;

extern supro_simulation_f32 supro_sim;  // declaration only

void supro_process(pipe *p);            // prototype only

#endif /* INC_SUPRO_SIMULATION_H_ */
