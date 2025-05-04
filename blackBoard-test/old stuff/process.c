/*
 * process.c
 *
 *  Created on: May 2, 2025
 *      Author: spencer
 */

#include "process.h"
#include "nodeSort.h"
#include <string.h>   /* memcpy, memset */

/* ------------------------------------------------------------------ */
/*  compile‑time guards                                               */
#ifndef VECTOR_SIZE
#  define VECTOR_SIZE  BUFFER_SIZE
#endif

/* ------------------------------------------------------------------ */
/*  elemental DSP helpers                                             */
/* ------------------------------------------------------------------ */
void addEffect      (const float *in, float *out, int p) {
    for (int i = 0; i < VECTOR_SIZE; ++i) out[i] = in[i] + (float)p;
}
void subtractEffect (const float *in, float *out, int p) {
    for (int i = 0; i < VECTOR_SIZE; ++i) out[i] = in[i] - (float)p;
}
void multiplyEffect (const float *in, float *out, int p) {
    for (int i = 0; i < VECTOR_SIZE; ++i) out[i] = in[i] * (float)p;
}
void divideEffect   (const float *in, float *out, int p) {
    float k = (p ? 1.0f / (float)p : 0.0f);
    for (int i = 0; i < VECTOR_SIZE; ++i) out[i] = in[i] * k;
}
void mixEffect(float *const inputs[], float *out,
               const float *w, int cnt)
{
    for (int i = 0; i < VECTOR_SIZE; ++i) {
        float sum = 0.0f, tot = 0.0f;
        for (int j = 0; j < cnt; ++j) {
            sum += inputs[j][i] * w[j];
            tot += w[j];
        }
        out[i] = tot ? sum / tot : 0.0f;
    }
}

/* ------------------------------------------------------------------ */
/*  internal: run graph in topological order                          */
/* ------------------------------------------------------------------ */
static void processGraph(dataLink *link,
                         const uint8_t *order,
                         size_t n,
                         pipeDef *pipe)
{
    for (size_t s = 0; s < n; ++s) {
        node *nd = &link->nodes[order[s]];

        /* gather input buffers */
        float *inPtr[MAX_INPUTS] = {0};
        for (uint8_t i = 0; i < nd->numInputs && i < MAX_INPUTS; ++i) {
            uint8_t src = nd->inputs[i];

            if (src == 254)          inPtr[i] = pipe->in1;
            else if (src == 255)     inPtr[i] = pipe->in2;
            else if (src < link->nodeCount)
                                     inPtr[i] = link->nodes[src].processBuffer;
        }

        /* perform effect */
        switch (nd->effectId)
        {
        case 255: /* passthrough: already handled prefill */
            break;

        case 0:  addEffect     (inPtr[0], nd->processBuffer, nd->params[0]); break;
        case 1:  subtractEffect(inPtr[0], nd->processBuffer, nd->params[0]); break;
        case 2:  multiplyEffect(inPtr[0], nd->processBuffer, nd->params[0]); break;
        case 3:  divideEffect  (inPtr[0], nd->processBuffer, nd->params[0]); break;

        case 6: {
            float w[MAX_INPUTS];
            for (uint8_t i = 0; i < nd->numInputs; ++i)
                w[i] = (float)nd->params[i];
            mixEffect(inPtr, nd->processBuffer, w, nd->numInputs);
            break;
        }

        default:
            memset(nd->processBuffer, 0, VECTOR_SIZE * sizeof(float));
            break;
        }
    }
}

/* ------------------------------------------------------------------ */
/*  public API                                                        */
/* ------------------------------------------------------------------ */
void processNetwork(dataLink *link, pipeDef *pipe, size_t len)
{
    if (!link || !pipe) return;
    if (len > VECTOR_SIZE) len = VECTOR_SIZE;

    /* pre‑load first two nodes (id 0 and 1) with external inputs if present */
    if (link->nodeCount > 0)
        memcpy(link->nodes[0].processBuffer, pipe->in1, len * sizeof(float));
    if (link->nodeCount > 1)
        memcpy(link->nodes[1].processBuffer, pipe->in2, len * sizeof(float));

    /* compute processing order */
    uint8_t order[MAX_NODES];
    size_t count = topoSort(link, order);

    /* run */
    processGraph(link, order, count, pipe);

    /* copy final outputs */
    if (link->nodeCount > 0)
        memcpy(pipe->out1, link->nodes[0].processBuffer,
               len * sizeof(float));
    if (link->nodeCount > 1)
        memcpy(pipe->out2, link->nodes[1].processBuffer,
               len * sizeof(float));
}
