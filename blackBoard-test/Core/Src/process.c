
/* process.c  --------------------------------------------------------
 *  Executes a dataLink graph into / out of a pipeDef buffer.
 * ------------------------------------------------------------------ */
#include <string.h>
#include "process.h"
#include "nodeSort.h"

/* Effect helpers --------------------------------------------------- */
static inline void addEffect      (const float *in, float *out, int p)
{ for (int i = 0; i < BUFFER_SIZE; ++i) out[i] = in[i] + p; }

static inline void subtractEffect (const float *in, float *out, int p)
{ for (int i = 0; i < BUFFER_SIZE; ++i) out[i] = in[i] - p; }

static inline void multiplyEffect (const float *in, float *out, int p)
{ for (int i = 0; i < BUFFER_SIZE; ++i) out[i] = in[i] * p; }

static inline void divideEffect   (const float *in, float *out, int p)
{ float k = p ? 1.0f/p : 0.0f; for (int i = 0; i < BUFFER_SIZE; ++i) out[i] = in[i] * k; }

static void mixEffect(float *const inputs[], float *out,
                      const float *w, uint8_t cnt)
{
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        float sum = 0.0f, tot = 0.0f;
        for (uint8_t j = 0; j < cnt; ++j) {
            sum += inputs[j][i] * w[j];
            tot += w[j];
        }
        out[i] = tot ? sum / tot : 0.0f;
    }
}

/* Internal --------------------------------------------------------- */
static void runGraph(dataLink *link, const uint8_t *order, size_t n, pipeDef *pipe)
{
    float *inPtr[MAX_INPUTS];

    for (size_t idx = 0; idx < n; ++idx) {
        node *nd = &link->nodes[order[idx]];

        /* Map inputs */
        for (uint8_t i = 0; i < nd->numInputs; ++i) {
            uint8_t src = nd->inputs[i];
            inPtr[i] = (src == 254) ? pipe->in1
                     : (src == 255) ? pipe->in2
                     : (src < link->nodeCount) ? link->nodes[src].processBuffer
                     : NULL;
        }

        /* Process */
        switch (nd->effectId) {
        case 255: /* passthrough: already pre‑filled */ break;
        case 0:  addEffect     (inPtr[0], nd->processBuffer, nd->params[0]); break;
        case 1:  subtractEffect(inPtr[0], nd->processBuffer, nd->params[0]); break;
        case 2:  multiplyEffect(inPtr[0], nd->processBuffer, nd->params[0]); break;
        case 3:  divideEffect  (inPtr[0], nd->processBuffer, nd->params[0]); break;
        case 6: {
            float w[MAX_INPUTS];
            for (uint8_t i = 0; i < nd->numInputs; ++i) w[i] = nd->params[i];
            mixEffect(inPtr, nd->processBuffer, w, nd->numInputs);
            break;
        }
        default:
            memset(nd->processBuffer, 0, BUFFER_SIZE*sizeof(float));
        }
    }
}

/* Public ----------------------------------------------------------- */
void processNetwork(dataLink *link, pipeDef *pipe, size_t len)
{
    if (len > BUFFER_SIZE) len = BUFFER_SIZE;

    /* preload first two nodes for passthrough */
    if (link->nodeCount > 0)
        memcpy(link->nodes[0].processBuffer, pipe->in1, len*sizeof(float));
    if (link->nodeCount > 1)
        memcpy(link->nodes[1].processBuffer, pipe->in2, len*sizeof(float));

    uint8_t order[MAX_NODES];
    size_t count = topoSort(link, order);

    runGraph(link, order, count, pipe);

    /* outputs */
    if (link->nodeCount > 0)
        memcpy(pipe->out1, link->nodes[0].processBuffer, len*sizeof(float));
    if (link->nodeCount > 1)
        memcpy(pipe->out2, link->nodes[1].processBuffer, len*sizeof(float));
}
