
/*  nodeSort.c  ------------------------------------------------------
 *  Simple Kahn topological sort (O(N+E)).
 *  ------------------------------------------------------------------ */
#include <string.h>
#include "nodeSort.h"

size_t topoSort(const dataLink *link, uint8_t *result)
{
    uint8_t indeg[MAX_NODES] = {0};
    uint8_t adj[MAX_NODES][MAX_NODES] = {0};
    uint8_t adjCnt[MAX_NODES] = {0};

    /* in‑degree & adjacency */
    for (uint8_t i = 0; i < link->nodeCount; ++i) {
        const node *n = &link->nodes[i];
        for (uint8_t j = 0; j < n->numInputs; ++j) {
            uint8_t src = n->inputs[j];
            if (src != 254 && src != 255 && src < link->nodeCount) {
                indeg[i]++;
                adj[src][adjCnt[src]++] = i;
            }
        }
    }

    uint8_t Q[MAX_NODES], h = 0, t = 0, out = 0;
    for (uint8_t i = 0; i < link->nodeCount; ++i)
        if (indeg[i] == 0) Q[t++] = i;

    while (h < t) {
        uint8_t curr = Q[h++];
        result[out++] = curr;
        for (uint8_t k = 0; k < adjCnt[curr]; ++k) {
            uint8_t child = adj[curr][k];
            if (--indeg[child] == 0)
                Q[t++] = child;
        }
    }
    return out; /* should equal nodeCount for an acyclic graph */
}
