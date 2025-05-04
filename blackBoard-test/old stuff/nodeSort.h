/*
 * nodeSort.h
 *
 *  Created on: May 3, 2025
 *      Author: spencer
 */

#ifndef SRC_NODESORT_H_
#define SRC_NODESORT_H_

#include "main.c"
#include "dataLink.h"
#include <stdint.h>

uint8_t nodeOrder[MAX_NODES];

void topoSort(const dataLink *link, uint8_t *result);






#endif /* SRC_NODESORT_H_ */
