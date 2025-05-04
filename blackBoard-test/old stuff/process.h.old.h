/*
 * process.h
 *
 *  Created on: May 2, 2025
 *      Author: spencer
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include "dataLink.h"

/* DSP block size */
#define VECTOR_SIZE   8

/* buffer pool bound = nodes + two inputs                             */
#define MAX_BUFFERS   MAX_NODES

/* run graph once – will recompute buffer plan each call */
void processNetwork(const dataLink *net,
                    const float    *sysIn0,          /* node‑0 samples */
                    const float    *sysIn1,          /* node‑1 samples */
                    uint8_t         len,             /* <= VECTOR_SIZE */
                    float          *outNode2,        /* first output   */
                    float          *outNode3);       /* second output  */

#endif /* PROCESS_H */
