# Yandex Interview Question (UPDATE: Positive response for next stage)
# Bit-Depth flexible Library for allocating fixed memory blocks
This is a simple C library of a block memory allocator. The allocator allows allocating and freeing one fixed size block from a static memory pool. The block and pool sizes are fixed during program execution, but are configurable during project build. The library is adapted to work on Embedded platforms of various bit depths. The allocator should work correctly in programs with RTOS preemptive multitasking due to the inclusion of a boolean mutex variable, which can be relaced. 
## Usage
1. Set the bit-width of your system to the required by setting `FMA_BIT_DEPTH` in `fxdMemAllocCfg.h`
``` cpp
#define FMA_BIT_DEPTH		32U
```

2. Call `#define FMALLOC_CTOR(objName, blockSz, nBlocks)` in your source file. The macro declares a static `tFMAlloc` struct, an associated buffer and a handler (pointer) with the name `objName` and reserves the aligned memory blocks of size `blockSz` and number `nBlocks`. Note that if `blockSz` is not aligned to `FMA_BIT_DEPTH`, the library automtically rounds-up the allocation for alignment.
```cpp
#include "fxdMemAlloc.h"
#define TEST_BLOCK_SIZE 35
#define TEST_BLOCK_NUM  15

/* Declares and initializes a static tFMAlloc handler (pointer) named test32 with
 given block size and blok number */
FMALLOC_CTOR(test32, TEST_BLOCK_SIZE, TEST_BLOCK_NUM); /* Pointer/handler with the name test32 is created */
```
3. For allocating a memory block, call the struct method `fn_alloc(phandler)` where `phandler` is a pointer to the `tFMAlloc` created in **2**. The fuction returns a pointer to the allocated memory block, or `NULL` if the handler is full. For freeing a block, call `vfn_free(phandler, pblock)`, where `pblock` is the block to be freed. See the example below.
``` cpp
#include "stdio.h"
#include "stdbool.h"
#include "time.h"
#include "stdlib.h"
#include "fxdMemAlloc.h"
#define TEST_BLOCK_SIZE 35
#define TEST_BLOCK_NUM  15

/* Declares and initializes a static tFMAlloc handler (pointer) named test32 with
 given block size and blok number */
FMALLOC_CTOR(test32, TEST_BLOCK_SIZE, TEST_BLOCK_NUM);

/* Holds [pointers to allocated blocks during tests */
uint8_t *ptr[TEST_BLOCK_NUM+2] = {NULL};

int main()
{
  /* Allocate some blocks till the buffer is full. Print the offsets from the buffer start address */
  for(int  i=0; i<TEST_BLOCK_NUM+2; i++){
    ptr[i] = test32->fn_alloc(test32);
    if(ptr[i] != NULL)
    {
      printf("%d :\tAllocated at %llu\n", i, ptr[i] - test32->pbuff);
    } else {
      printf("%d :\tAllocation failed\n", i);
    }
  }
  srand(time(NULL));
  int r;
  
  /* Randomly deallocate(free) half of allocated buffers and print the offsets */
  for(int  i=0; i<TEST_BLOCK_NUM/2;){
    r = rand() % TEST_BLOCK_NUM;
    if(ptr[r] != NULL){
      printf("%d :\tFreed pointer at offset %llu\n",r, ptr[r] - test32->pbuff);
      test32->vfn_free(test32, ptr[r]);
      ptr[r] = NULL;
      i++;
    }
  }
  /* Reallocate the allocated buffers. The offsets should be the same as those above, but in opposite order. See example output */
  for(int  i=0; i<TEST_BLOCK_NUM; i++){
    if(ptr[i] == NULL)
    {
      ptr[i] = test32->fn_alloc(test32);
      if(ptr[i] != NULL)
      {
        printf("%d :\tAllocated at %llu\n", i, ptr[i] - test32->pbuff);
      } else {
        printf("%d :\tAllocation failed\n", i);
      }
    }
  }
  return 0;
}
```

<p> 
<p>An example of the output is given in the image below <p>
![Allocator output](https://user-images.githubusercontent.com/20101804/216818995-a2804447-6fd6-46d2-bb1d-fdac998c9be4.png)

