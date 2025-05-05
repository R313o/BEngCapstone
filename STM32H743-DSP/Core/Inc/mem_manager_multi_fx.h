
#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include "main.h"

void dctm_pool_init();
void static_pool_init();

void *_dctm_static_mem_alloc(size_t size, size_t align);
void *_static_mem_alloc(size_t size, size_t align);

#endif /* MEM_MANAGER_H */
