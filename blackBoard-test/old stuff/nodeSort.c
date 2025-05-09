/*
 * nodeSort.c
 *
 *  Created on: May 3, 2025
 *      Author: spencer
 */
#include "nodeSort.h"

void topoSort(const dataLink *link, uint8_t *result)
{
    uint8_t inDegree[MAX_NODES] = {0};
    uint8_t dependents[MAX_NODES][MAX_NODES];
    uint8_t depCount[MAX_NODES] = {0};

    /* Build in‑degree table and adjacency lists */
    for (uint8_t i = 0; i < link->nodeCount; ++i) {
        const node *n = &link->nodes[i];
        for (uint8_t j = 0; j < n->numInputs; ++j) {
            uint8_t inputId = n->inputs[j];
            if (inputId != NULL_INPUT && inputId < link->nodeCount) {
                inDegree[i]++;
                dependents[inputId][depCount[inputId]++] = i;
            }
        }
    }

    uint8_t queue[MAX_NODES];
    uint8_t head = 0, tail = 0;

    for (uint8_t i = 0; i < link->nodeCount; ++i) {
        if (inDegree[i] == 0)
            queue[tail++] = i;
    }

    size_t outCount = 0;
    while (head < tail) {
        uint8_t curr = queue[head++];
        result[outCount++] = curr;

        for (uint8_t k = 0; k < depCount[curr]; ++k) {
            uint8_t child = dependents[curr][k];
            if (--inDegree[child] == 0)
                queue[tail++] = child;
        }
    }
}
