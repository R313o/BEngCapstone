/*
 * dataLink.h
 *
 *  Created on: May 2, 2025
 *      Author: spencer
 */

#ifndef SRC_DATALINK_H_
#define SRC_DATALINK_H_

#include "main.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef USER_NODES
  #define USER_NODES   20
#endif

#ifndef MAX_INPUTS
  #define MAX_INPUTS   4
#endif
#ifndef NUM_PARAMETERS
  #define NUM_PARAMETERS 3
#endif


typedef struct dataLink dataLink;

typedef void (*readUART_Fn)(dataLink* link, char c);


typedef struct {
    uint8_t nodeId;
    uint8_t effectId;
    uint8_t numInputs;
    uint8_t params[NUM_PARAMETERS];
    uint8_t inputs[MAX_INPUTS];
    float32_t processBuffer[128];
} node;


struct dataLink {
    node     nodes[MAX_NODES];
    uint8_t  nodeCount;
    char     commandBuffer[128];
    uint8_t  commandIndex;
    readUART_Fn readUART;
};


void  initDataLink(dataLink* link);
void  readUART   (dataLink* link,char c);

#endif /* SRC_DATALINK_H_ */
