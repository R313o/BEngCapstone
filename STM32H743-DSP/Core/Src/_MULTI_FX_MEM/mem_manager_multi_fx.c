/**
 * @file    mem_manager_multi_fx.c
 * @brief   Static memory pool allocators for multiFX engine (DTCM + SRAM).
 *
 * This module provides:
 *   - A fixedsize DTCM allocator for audio buffers (BUFFER_SIZE floats).
 *   - A fixedsize SRAM allocator for FFT buffers (FFT_SIZE floats).
 *   - A simple align_up() helper for alignment of allocations.
 *   - Initialization routines for both pools.
 *   - An outofmemory handler that loops indefinitely.
 *
 * @author  Hassan Islam
 * @date    May 2025
 */

#include "mem_manager_multi_fx.h"
#include "pipe.h"


/**
 * @brief Default out of memory handler.
 *
 * Enters an infinite loop to halt execution; typically caught in the debugger
 * or reset by a watchdog. This function never returns.
 */
static void _memory_alloc_error_handler(void);


/**
 * @brief Backing store for DTCM allocations.
 *        Placed in the .dtcm section and 32byte aligned.
 */
__attribute__((section(".dtcm"), aligned(32)))
static uint8_t dtcm_static_pool[DTCM_STATIC_POOL_SIZE];

/**
 * @brief Current head offset into dtcm_static_pool.
 *        Must be manually zeroed at startup (noload section).
 */
__attribute__((section(".dtcm")))
static size_t dtcm_pool_head = 0u;

/**
 * @brief Backing store for static SRAM allocations.
 *        Holds FFT_SIZEsized buffers.
 */
static uint8_t static_pool[STATIC_POOL_SIZE];

/**
 * @brief Current head offset into static_pool.
 */
static size_t pool_head = 0u;


/**
 * @brief Aligns a value upward to the next multiple of `align`.
 *
 * @param x     The original value (size or offset).
 * @param align The alignment boundary (must be a power of two).
 * @return The smallest value ≥ x that is a multiple of align.
 */
static inline size_t align_up(size_t x, size_t align)
{
    return (x + (align - 1u)) & ~(align - 1u);
}

/**
 * @brief Initialize the DTCM memory pool.
 *
 * Clears the dtcm_static_pool to zero and resets the head pointer.
 * Must be called once at system startup before any DTCM allocations.
 */
void dctm_pool_init()
{
    dtcm_pool_head = 0u;
    memset(dtcm_static_pool, 0, DTCM_STATIC_POOL_SIZE);
}

/**
 * @brief Initialize the static SRAM memory pool.
 *
 * Clears the static_pool to zero and resets the head pointer.
 * Must be called once at system startup before any SRAM allocations.
 */
void static_pool_init()
{
    pool_head = 0u;
    memset(static_pool, 0, STATIC_POOL_SIZE);
}

/**
 * @brief Allocate a block of memory from the DTCM pool.
 *
 * @param size  Number of bytes to allocate.
 * @param align Required byte alignment for the returned pointer.
 * @return Pointer to the allocated block within dtcm_static_pool.
 *         On overflow, calls _memory_alloc_error_handler().
 */
void *_dctm_static_mem_alloc(size_t size, size_t align)
{
    size_t off = align_up(dtcm_pool_head, align);

    if (off + size > DTCM_STATIC_POOL_SIZE) {
        _memory_alloc_error_handler();
    }

    dtcm_pool_head = off + size;
    return &dtcm_static_pool[off];
}

/**
 * @brief Allocate a block of memory from the static SRAM pool.
 *
 * @param size  Number of bytes to allocate.
 * @param align Required byte alignment for the returned pointer.
 * @return Pointer to the allocated block within static_pool.
 *         On overflow, calls _memory_alloc_error_handler().
 */
void *_static_mem_alloc(size_t size, size_t align)
{
    size_t off = align_up(pool_head, align);

    if (off + size > STATIC_POOL_SIZE) {
        _memory_alloc_error_handler();
    }

    pool_head = off + size;
    return &static_pool[off];
}



/**
 * @brief Default handler for outofmemory conditions.
 *
 * Enters an infinite loop to halt execution; suitable
 * for catching in the debugger or watchdog reset.
 */
static void _memory_alloc_error_handler()
{
    while (1) {
        // handler error
    }
}
