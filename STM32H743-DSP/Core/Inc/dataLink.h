
/*  dataLink.h  -------------------------------------------------------
 *  Node definition and container.
 *  ------------------------------------------------------------------ */
#ifndef DATALINK_H
#define DATALINK_H

#include "config.h"
//#include "main.h"


/* Forward declaration so we can reference it in the callback typedef */
struct dataLink;

typedef struct dataLink dataLink;
/* UART line‑by‑line callback: user supplies implementation */
typedef void (*readUART_Fn)(dataLink *link, char c);

/* ---------------- Node -------------------------------------------- */
typedef struct {
    uint8_t nodeId;
    uint8_t effectId;                   /* 0 add, 1 sub, 2 mul, 3 div, 6 mix, 255 input */
    uint8_t numInputs;
    uint8_t params[NUM_PARAMETERS];
    uint8_t inputs[MAX_INPUTS];
    //float32_t  processBuffer[BUFFER_SIZE];
} node;

/* ---------------- Graph container --------------------------------- */
typedef struct dataLink {
    node     nodes[MAX_NODES];
    uint8_t  nodeCount;
    char     commandBuffer[128];
    uint8_t  commandIndex;
    uint8_t  processOrder[MAX_NODES];
    uint8_t  needsRefresh;


    readUART_Fn readUART;
} dataLink;

void initDataLink(dataLink *link);

size_t topoSort(dataLink *link, uint8_t *result);


#endif /* DATALINK_H */
