
/* process.h ---------------------------------------------------------
 * DSP network runner — uses a single pipeDef I/O structure.
 * ------------------------------------------------------------------ */
#ifndef PROCESS_H
#define PROCESS_H

#include "config.h"
#include "dataLink.h"
#include <stddef.h>

typedef struct {
    float in1[BUFFER_SIZE];
    float in2[BUFFER_SIZE];
    float out1[BUFFER_SIZE];
    float out2[BUFFER_SIZE];
} pipeDef;

void processNetwork(dataLink *link, pipeDef *pipe, size_t len);

#endif /* PROCESS_H */
