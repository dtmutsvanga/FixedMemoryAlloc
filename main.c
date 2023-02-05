#include <stdio.h>
#include "fxdMemAlloc.h"
#include "stdbool.h"
#include <time.h>
#include <stdlib.h>
#define TEST_BLOCK_SIZE 35
#define TEST_BLOCK_NUM  15

/* Declares and initializes a static tFMAlloc handler (pointer) named test32 with
 given block size and blok number */
FMALLOC_CTOR(test32, TEST_BLOCK_SIZE, TEST_BLOCK_NUM);

/* Holds [pointers to allocated blocks during tests */
uint8_t *ptr[TEST_BLOCK_NUM] = {NULL};

int main()
{
  srand(time(NULL));
  int r;
  for(int  i=0; i<TEST_BLOCK_NUM; i++){
    ptr[i] = test32->fn_alloc(test32);
    if(ptr[i] != NULL)
    {
      printf("%d :\tAllocated at %llu\n", i, ptr[i] - test32->pbuff);
    } else {
      printf("%d :\tAllocation failed\n", i);
    }
  }

  for(int  i=0; i<TEST_BLOCK_NUM/2;){
    r = rand() % TEST_BLOCK_NUM;
    if(ptr[r] != NULL){
      printf("%d :\tFreed pointer at offset %llu\n",r, ptr[r] - test32->pbuff);
      test32->vfn_free(test32, ptr[r]);
      ptr[r] = NULL;
      i++;
    }
  }

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
