/**
 * @file    mem_manager.h
 * @brief   Simple static memory pool allocators for DTCM and SRAM.
 *
 * Provides two fixed-size pools:
 *   - A DTCM pool for BUFFER_SIZE sized float buffers.
 *   - A regular SRAM pool for FFT_SIZE sized float buffers.
 *
 * Usage:
 *   1. Call dctm_pool_init() and static_pool_init() once at startup.
 *   2. Allocate from DTCM via _dctm_static_mem_alloc(size, align).
 *   3. Allocate from SRAM via _static_mem_alloc(size, align).
 *   4. On overflow, the error handler _memory_alloc_error_handler() loops forever.
 *
 * @note   BUFFER_SIZE and FFT_SIZE must be defined before including this header.
 */

#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include "main.h"   /* for BUFFER_SIZE, FFT_SIZE */

/** @brief Number of float sized buffers in the DTCM pool. */
#define DTCM_NUM_BUFFERS      15

/** @brief Total DTCM pool size in bytes: DTCM_NUM_BUFFERS × BUFFER_SIZE floats. */
#define DTCM_STATIC_POOL_SIZE (DTCM_NUM_BUFFERS * BUFFER_SIZE * sizeof(float))

/** @brief Number of FFT sized buffers in the static SRAM pool. */
#define NUM_BUFFERS           52

/** @brief Total SRAM pool size in bytes: NUM_BUFFERS × FFT_SIZE floats. */
#define STATIC_POOL_SIZE      (NUM_BUFFERS * FFT_SIZE * sizeof(float)) + 4

/** @brief Number of FFT sized buffers in the static SRAM pool. */
#define NUM_BUFFERS_RAM_D2     20

/** @brief Total SRAM pool size in bytes: NUM_BUFFERS × FFT_SIZE floats. */
#define STATIC_POOL_SIZE_RAM_D2      (NUM_BUFFERS_RAM_D2 * FFT_SIZE * sizeof(float)) + 4



/**
 * @brief Initialize the DTCM memory pool.
 *
 * Clears the internal DTCM buffer and resets the allocation pointer.
 * Must be called once before any calls to _dctm_static_mem_alloc().
 */
void dctm_pool_init(void);

/**
 * @brief Initialize the static SRAM memory pool.
 *
 * Clears the internal SRAM buffer and resets the allocation pointer.
 * Must be called once before any calls to _static_mem_alloc().
 */
void static_pool_init(void);


/**
 * @brief Allocate a block from the DTCM pool.
 *
 * @param size  Number of bytes to allocate.
 * @param align Required alignment in bytes (power of two).
 * @return Pointer to the aligned block within DTCM_STATIC_POOL.
 *         On overflow, calls _memory_alloc_error_handler().
 */
void *_dctm_static_mem_alloc(size_t size, size_t align);

/**
 * @brief Allocate a block from the static SRAM pool.
 *
 * @param size  Number of bytes to allocate.
 * @param align Required alignment in bytes (power of two).
 * @return Pointer to the aligned block within STATIC_POOL.
 *         On overflow, calls _memory_alloc_error_handler().
 */
void *_static_mem_alloc(size_t size, size_t align);
void *_static_mem_alloc_ram_d2(size_t size, size_t align);



#endif /* MEM_MANAGER_H */
