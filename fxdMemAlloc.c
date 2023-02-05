/*
 * fxdMemAlloc.c
 *
 *  Created on: Feb 4, 2023
 *      Author: dtmut
 */

#include "fxdMemAlloc.h"
#include <stdbool.h>
/**
 * @brief Checks if a block is within the bounds of the memory pool
 * 
 * @param self self pointer to \ref tFMAlloc object
 * @param pblock poiner to the memory block
 * @return true if the block is within the pool
 * @return false otherwise
 */
static inline bool isBlockInMemPool(tFMAlloc *self, void *pblock)
{
    return (self->pbuff <= (uint8_t*)pblock) &&  ((uint8_t*)pblock + self->blockSize <= self->pbuff + self->bufflen);
}
/**
 * @brief Get the Block By Idx object
 * 
 * @param self self pointer to \ref tFMAlloc object
 * @param idx 
 * @return tFMAllocBlck* Pointer to the block of memory
 */
static inline tFMAllocBlck *getBlockByIdx(tFMAlloc *self, tFMAllocBlck idx){
    if((size_t)idx >= self->nBlocks) return NULL;
    return (tFMAllocBlck *) &self->pbuff[self->blockSize * (size_t)idx];
}

/**
 * @brief Get the index of a block in the pool using the block pointer
 * 
 * @param self self pointer to \ref tFMAlloc object
 * @param pblock pointer to the block
 * @return size_t index of the block in the pool
 */
static inline size_t getIdxByBlockPtr(tFMAlloc *self, void *pblock)
{
    return ((uint8_t *)pblock - self->pbuff) / self->blockSize;
}
/**
 * @brief Set the index of the next block to the next uninitialized block, if we haven't initialized all blocks yet
 * @details This funtion enables us to avoid initializing the whole buffer at startup.
 * @param self self pointer to \ref tFMAlloc object
 */
static inline void setNxtUnintdBlockPtr(tFMAlloc *self) {
     if(self->initdBolcks < self->nBlocks )
    {
        /* Store the index of the next block on the next free uninitialized block */
        tFMAllocBlck *p = getBlockByIdx(self, self->initdBolcks);
        if(p == NULL) return; /* Index too large */
        self->initdBolcks++;
        *p = (tFMAllocBlck)self->initdBolcks;
    } 
}
/**
 * @brief Allocates a memory block of the defined size 
 * 
 * @param self pointer to \ref tFMAlloc object
 * @return void* pointer to allocated me,ory block
 */
void *FMA_Alloc(tFMAlloc *self){
    FMAUTIL_ASSERT(self != NULL);
    void *retval = NULL;
    FMA_MUX_TAKE
    setNxtUnintdBlockPtr(self);
    if(self->freeBlocks > 0){
      retval = self->nextBlock;
      self->freeBlocks -= 1;
      /* Set next block. Current next block contains the index of the following next block */
      self->nextBlock = (self->freeBlocks == 0) ? NULL : getBlockByIdx(self, *(tFMAllocBlck *)(self->nextBlock));
    }
    FMA_MUX_GIVE;
    return retval;
}

/**
 * @brief Frees a previously allicated memory block 
 * 
 * @param self pointer to \ref tFMAlloc object 
 * @param pblock pointer to memory block to be freed 
 */
void FMA_free(tFMAlloc *self, void *pblock)
{
    FMAUTIL_ASSERT(self != NULL);
    FMAUTIL_ASSERT(pblock != NULL);
    FMAUTIL_ASSERT(isBlockInMemPool(self, pblock));
    FMA_MUX_TAKE;
    /* For freeing, replace the next block index with the pblock, and set the pblock to point to the previous next */
    *(tFMAllocBlck *)pblock = self->nextBlock == NULL? (tFMAllocBlck)self->nBlocks :  getIdxByBlockPtr(self, self->nextBlock);
    self->nextBlock = pblock;
    self->freeBlocks += 1;
    FMA_MUX_GIVE;
}
/**
 * @brief Reset the buffer to the initial state.
 * @note this function doesn't zero the buffer
 * @param self 
 */
void FMA_reset(tFMAlloc *self){
    FMA_MUX_TAKE;
    FMAUTIL_ASSERT(self != NULL);
    self->nextBlock = (tFMAllocBlck *)self->pbuff;
    self->freeBlocks = self->nBlocks;
    self->initdBolcks = 0;
    FMA_MUX_GIVE;
}
