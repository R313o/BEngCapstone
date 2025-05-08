
/* dataLink.c --------------------------------------------------------
 * Very small helper for parsing UART‑received ASCII into the graph.
 * ------------------------------------------------------------------ */
#include "dataLink.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Example implementation: splits node definitions on ';' */
void readUART(dataLink* link, char c)
{
    if (c == '\n' || c == '\r') {
        link->commandBuffer[link->commandIndex] = '\0';
        link->commandIndex = 0;

        char* token;
        char* rest = link->commandBuffer;

        uint8_t nodeId = 255;
        uint8_t effectId = 5;
        uint8_t params[NUM_PARAMETERS];
        uint8_t inputs[MAX_INPUTS];
        memset(params, 255, sizeof params);   /* all 255, not just first */
        memset(inputs, 255, sizeof inputs);

        while ((token = strtok_r(rest, ";", &rest))) {
            if (token[0] == 'N')
            {
            	if(atoi(&token[1]) == 0)
            	{
            		link->needsRefresh = 2;
            		initDataLink(link);
            	}
                nodeId = atoi(&token[1]);
            }
            else if (token[0] == 'E')
            {
                effectId = atoi(&token[1]);
            }
            else if (token[0] == 'P')
            {
                char* p = &token[1];
                uint8_t i = 0;
                while (*p && i < NUM_PARAMETERS)
                {
                    params[i++] = atoi(p);
                    while (*p && *p != ',') p++;
                    if (*p == ',')
                    {
                    	p++;
                    }
                }
            } else if (token[0] == 'I')
            {
                char* p = &token[1];
                for (uint8_t i = 0; i < MAX_INPUTS && *p; )
                {
					if (!isdigit((unsigned char)*p)) { p++; continue; }
					inputs[i++] = (uint8_t) strtoul(p, &p, 10);
					if (*p == ',') p++;
                }
            }
        	else if (token[0] == 'T')
            {
        		link->needsRefresh = 1;
            }
        }

        if (nodeId < MAX_NODES)
        {
            node* n = &link->nodes[nodeId];
            n->nodeId = nodeId;
            n->effectId = effectId;

            for (uint8_t i = 0; i < NUM_PARAMETERS; i++)
            {
            	n->params[i] = params[i];
            }

            uint8_t inputCount = 0;
            for (uint8_t i = 0; i < MAX_INPUTS; i++)
            {
            	n->inputs[i] = inputs[i];
            	if (inputs[i] != 255) inputCount++;
            }
            n->numInputs = inputCount;

            if (nodeId >= link->nodeCount)
            {
            	link->nodeCount = nodeId + 1;
            }
        }
        return;
    }

    if (link->commandIndex < sizeof(link->commandBuffer) - 1)
    {
        link->commandBuffer[link->commandIndex++] = c;
    }

    //link->needsRefresh = 1;
}

size_t topoSort(dataLink *link, uint8_t *result)
{
    uint8_t indeg[MAX_NODES] = {0};
    uint8_t adj[MAX_NODES][MAX_NODES] = {0};
    uint8_t adjCnt[MAX_NODES] = {0};


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
        link->processOrder[out++] = curr;
        for (uint8_t k = 0; k < adjCnt[curr]; ++k) {
            uint8_t child = adj[curr][k];
            if (--indeg[child] == 0)
                Q[t++] = child;
        }
    }

    return out;
}


void initDataLink(dataLink* link)
{
    link->nodeCount = 0;
    //link->isComplete = 0;
    link->commandIndex = 0;

    link->readUART = readUART;

    link->needsRefresh = 0;

    for (int i = 0; i < MAX_NODES; i++)
    {

        link->nodes[i].nodeId = 255;
        link->nodes[i].effectId = 5;
        link->nodes[i].numInputs = 0;
        link->processOrder[i] = i;

        for (int j = 0; j < NUM_PARAMETERS; j++)
        {
            link->nodes[i].params[j] = 255;
        }

        for (int j = 0; j < MAX_INPUTS; j++)
        {
            link->nodes[i].inputs[j] = 255;
        }

    }
}
