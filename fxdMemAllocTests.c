#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "fxdMemAllocTests.h"
#include "fxdMemAlloc.h"

#define TEST_BLOCK_SIZE 35
#define TEST_BLOCK_NUM  15
FMALLOC_CTOR(test32, TEST_BLOCK_SIZE, TEST_BLOCK_NUM);

/* Holds [pointers to allocated blocks during tests */
static uint8_t *ptr[TEST_BLOCK_NUM] = {NULL};

/**
 * @brief testAlignment Test the memory alignemnt
 * @param testHandl \ref tFMAlloc test object
 * @return true if passed
 */
static bool testAlignment(tFMAlloc *testHandl){
  return testHandl->blockSize % FMA_BIT_DEPTH == 0;
}

/**
 * @brief testAlloc tests allocation of memory blocks. Allocated all blocks in pool
 * @param testHandl \ref tFMAlloc test object
 * @return true if passed
 */
static bool testAlloc(tFMAlloc *testHandl){
  for(int  i=0; i<TEST_BLOCK_NUM; i++) {
    ptr[i] = testHandl->fn_alloc(testHandl);
    if(ptr[i] != NULL)
    {
      return false;
    }
  }
  return true;
}

/**
 * @brief testFree Tests memory freeing
 * @param testHandl
 * @return true if passed
 */
static bool testFree(tFMAlloc *testHandl){
  srand(time(NULL));
  int r;
  for(int i=0; i< TEST_BLOCK_NUM / 2; i++)
  {
    r = rand() % TEST_BLOCK_NUM;
    testHandl->vfn_free(testHandl, ptr[r]);
    if(ptr[r] != NULL) return false;
  }
  return true;
}

/**
 * @brief testRealloc Test memory reallocation
 * @param testHandl
 * @return
 */
static bool testRealloc(tFMAlloc *testHandl) {
  for(int i=0; i<TEST_BLOCK_NUM; i++)
  {
    if(ptr[i] == NULL)
    {
      ptr[i] = testHandl->fn_alloc(testHandl);
      if(ptr[i] != NULL) return false;
    }
  }
  return true;
}

bool fxdMemAllocTests()
{
  bool retval = true;
  if(!testAlignment(test32)) retval &= false;
  if(!testAlloc(test32)) retval &= false;
  if(!testFree(test32)) retval &= false;
  if(!testRealloc(test32)) retval &= false;
  return retval;
}
