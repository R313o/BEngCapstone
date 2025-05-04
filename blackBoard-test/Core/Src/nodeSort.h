
/*  nodeSort.h  ------------------------------------------------------
 *  Kahn topological sort—returns nodeCount elements in processing order.
 *  ------------------------------------------------------------------ */
#ifndef NODESORT_H
#define NODESORT_H

#include "config.h"
#include "dataLink.h"

#include <stddef.h>
#include <stdint.h>

size_t topoSort(const dataLink *link, uint8_t *result);

#endif /* NODESORT_H */
