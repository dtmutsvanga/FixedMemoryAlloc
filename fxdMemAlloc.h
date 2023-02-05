/*
 * fxdMemAlloc.h
 *
 *  Created on: Feb 4, 2023
 *      Author: dtmut
 */

#ifndef FXDMEMALLOC_H_
#define FXDMEMALLOC_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fxdMemAllocUtils.h"
#include "fxdMemAllocCfg.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef FMA_BIT_DEPTH
#warning "Please define FMA_BIT_DEPTH in fxdMemAllocCfg.h. Defaulting to 32 bits"
#define FMA_BIT_DEPTH	32
#endif
/*** MUTEX ***/
#define FMA_MUX_TYPE        bool                            /* Mutex type. Assumes boolean access is atomic. Replace with RTOS mux */
#define FMA_MUX_TAKE   {do { } while ((self->mux) == 0); (self->mux) = false;} /* Take mutex. Wait and set the mutex to zero */
#define FMA_MUX_GIVE   {(self->mux) = true;}
#define FMA_MIN_BLOCK_SIZE	(sizeof(tFMAllocBlck*))					/* Minimum block size. The first sizeof(tFMAlloc*) of a free block has a pointer to the next block */

#define tFMAllocBlck		uint32_t
/* Align a given block size the predefined system bit depth
 * FMAUTIL_ALIGN handles memory alignment */
#define FMA_BLCK_SIZE_ALIGN(blockSiz) ((FMAUTIL_ALIGN(blockSiz, FMA_BIT_DEPTH) > FMA_MIN_BLOCK_SIZE) ? \
										FMAUTIL_ALIGN(blockSiz, FMA_BIT_DEPTH) : \
										FMA_MIN_BLOCK_SIZE)
#define FMA_BUFF_TYPE 	uint8_t									/* Memory pool base data type */
typedef struct sFMAlloc tFMAlloc;
struct sFMAlloc
{
  FMA_MUX_TYPE  mux;                        /* Mutex. Assumes access is atomic */
  const size_t 	nBlocks;                    /* Number of blocks*/
	const size_t 	blockSize;									/* Block size */
  const size_t 	bufflen;                    /* Length of pbuff */
  size_t 			freeBlocks;                   /* Number of free blocks */
  size_t 			initdBolcks;                  /* Number of initialized blocks */
  FMA_BUFF_TYPE	* const pbuff;							/* Memory pool of length bufflen bytes */
	tFMAllocBlck	*nextBlock;									/* Next memory block for allocation */
  void* 			(*fn_alloc)(tFMAlloc *self);	/* Method  for single block allocation */
  void 			(*vfn_free )(tFMAlloc *self, void *pblock);	/* Method for single block deallocation */
  void 			(*vfn_reset)(tFMAlloc *self);		/* Method for resetting the allocator obect */
}sFMAlloc;

/**
 * @brief Allocates a memory block of the defined size
 *
 * @param self pointer to \ref tFMAlloc object
 * @return void* pointer to allocated me,ory block
 */
void *FMA_Alloc(tFMAlloc *self);
/**
 * @brief Frees a previously allicated memory block
 *
 * @param self pointer to \ref tFMAlloc object
 * @param pblock pointer to memory block to be freed
 */
void FMA_free(tFMAlloc *self, void *pblock);
/**
 * @brief Reset the buffer to the initial state.
 * @note this function doesn't zero the buffer
 * @param self
 */
void FMA_reset(tFMAlloc *self);

/**
 * @brief Constructor for Fixed memory allocations. 
 * @detail Creates an object of type \ref tFMAlloc with the name objName_FMAHandle and an associated static buffer
 * @param objName name of the object 
 * @param blockSz block size. Ideally should be a multipe of your bit depth as defined by \ref FMA_BIT_DEPTH. Unaligned blocks 
 *			are automatically aligned.
 * @param nBlocks number of memory blocks 
 * */
#define FMALLOC_CTOR(objName, blockSz, nBlocks) \
static FMA_BUFF_TYPE objName##_Buff[FMA_BLCK_SIZE_ALIGN(blockSz) * nBlocks] = {0}; 	\
static tFMAlloc objName##_FMAHandle = {												\
							true, \
							nBlocks,						\
							FMA_BLCK_SIZE_ALIGN(blockSz),\
							FMA_BLCK_SIZE_ALIGN(blockSz) * nBlocks,\
							nBlocks,						\
							0,							\
							objName##_Buff,				\
							(tFMAllocBlck*)objName##_Buff,\
							FMA_Alloc,					\
							FMA_free,						\
							FMA_reset						\
            };\
static tFMAlloc * const objName = &objName##_FMAHandle;

#ifdef __cplusplus
}
#endif //__cplusplus
#endif /* FXDMEMALLOC_H_ */
