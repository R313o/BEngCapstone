
#include "mem_manager_multi_fx.h"
#include "pipe.h"


#define DTCM_NUM_BUFFERS 3
#define DTCM_STATIC_POOL_SIZE  (DTCM_NUM_BUFFERS * BUFFER_SIZE * sizeof(float))

#define NUM_BUFFERS 48
#define STATIC_POOL_SIZE  (NUM_BUFFERS * FFT_SIZE * sizeof(float))

__attribute__((section(".dtcm"), aligned(32)))  static uint8_t dtcm_static_pool[DTCM_STATIC_POOL_SIZE];
__attribute__((section(".dtcm"))) 				static size_t  dtcm_pool_head = 0u;

												static uint8_t static_pool[STATIC_POOL_SIZE];
												static size_t  pool_head = 0u;



inline size_t align_up(size_t x, size_t align);

inline size_t align_up(size_t x, size_t align)
{
    return (x + (align - 1u)) & ~(align - 1u);
}

void dctm_pool_init()
{
   dtcm_pool_head = 0;  // dtcm is a no-load section of ram thus must be manually initialized to zero
   memset(dtcm_static_pool, (uint8_t)0, DTCM_STATIC_POOL_SIZE);
}


void static_pool_init()
{
   pool_head = 0;
   memset(static_pool, (uint8_t)0, STATIC_POOL_SIZE);
}


void *_dctm_static_mem_alloc(size_t size, size_t align)
{
    size_t off = align_up(dtcm_pool_head, align);
    if (off + size > DTCM_STATIC_POOL_SIZE)
    	_memory_alloc_error_handler();// return NULL;                             // out of memory
    dtcm_pool_head = off + size;
    return &dtcm_static_pool[off];
}


void *_static_mem_alloc(size_t size, size_t align)
{
    size_t off = align_up(pool_head, align);
    if (off + size > STATIC_POOL_SIZE)
    	_memory_alloc_error_handler(); //return NULL;                             // out of memory
    pool_head = off + size;
    return &static_pool[off];
}



void _memory_alloc_error_handler(){

	while(1);

}
