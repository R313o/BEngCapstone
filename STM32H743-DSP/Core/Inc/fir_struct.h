/*
 * fir_struct.h
 *
 *  Created on: Apr 28, 2025
 *      Author: Hassa
 */

#include <stdint.h>

#ifndef INC_FIR_STRUCT_H_
#define INC_FIR_STRUCT_H_


typedef struct {
    const float* ir_ffts[46];
    float*       prev_ffts[46];
    int32_t      curr_fftidx;
    int32_t      prev_fftidx;
    int32_t      numSegments;
} fir_t;


#endif /* INC_FIR_STRUCT_H_ */
