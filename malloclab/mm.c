/*
 * mm.c -- implement your malloc functions here. See mm-naive.c for an example implementation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm-common.h"
#include "memlib.h"
#include "mm.h"

static char *heap_listp;

header_t *
first_fit(size_t csz)
{
	void *ptr;
   /* first fit search */
    for (ptr = heap_listp; GET_SIZE(HDRP(ptr)) >= 0; ptr = NEXT_BLKP(ptr)) {
        if (csz <= GET_SIZE(HDRP(ptr))) {
            return ptr;
        }
    }
	return NULL; //no fit
}

header_t *
ask_os_for_chunk(size_t csz)
{
	header_t *bp;
    size_t size;
    
    /* Allocate an even number of words to maintain alignment */
    size = (csz % 2) ? (csz+1) * WSIZE : csz * WSIZE;
    //if ((int)(bp = mem_sbrk(size)) == -1) 
  //      return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* new epilogue header */

    /* Coalesce if the previous block was free */
    coalesce(bp);
    return bp; 
}

void
coalesce(header_t *h)
{
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(h)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(h)));
  size_t size = GET_SIZE(HDRP(h));

  //if (prev_alloc && next_alloc) {            /* Case 1 */
  //  return 0;
  //}
  
  if (!next_alloc) {      /* Case 2 */
    size += GET_SIZE(HDRP(NEXT_BLKP(h)));
    PUT(HDRP(h), PACK(size, 0));
    PUT(FTRP(h), PACK(size,0));
    //return h; 
  }

  else if (!prev_alloc) {      /* Case 3 */
    size += GET_SIZE(HDRP(PREV_BLKP(h)));
    PUT(FTRP(h), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(h)), PACK(size, 0));
    //return(PREV_BLKP(h)); 
    PREV_BLKP(h);

    }

  else {                                     /* Case 4 */
    size += GET_SIZE(HDRP(PREV_BLKP(h))) + GET_SIZE(FTRP(NEXT_BLKP(h)));
    PUT(HDRP(PREV_BLKP(h)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(h)), PACK(size, 0));
    //return(PREV_BLKP(h)); 
    PREV_BLKP(h);
    }
  //return add_chunk(h); 
}




/* 
 * mm_init initializes the malloc package.
 */
int mm_init(void)
{
	char *heap_listp; 
	char *free; 

	if((heap_listp = mem_sbrk(64)) == NULL) {
		return -1; 
	}

	PUT(heap_listp, 0);
	PUT(heap_listp + WSIZE, PACK(DSIZE, 1));
	PUT(heap_listp + DSIZE, PACK(DSIZE, 1));
	PUT(heap_listp + 24, PACK(0, 1));
	free = heap_listp + 16;  

	if(extend_heap(4) == NULL){ 
		return -1;
	}

	return 0;
}



void *mm_malloc(size_t size)
{
  size_t asize;      /* adjusted block size */
  size_t extendsize; /* amount to extend heap if no fit */
  char *bp;      
  if (heap_listp == 0){
    mm_init();
  }

  /* Ignore spurious requests */
  if (size <= 0)
    return NULL;

  /* Adjust block size to include overhead and alignment reqs. */
  if (size <= DSIZE)
    asize = DSIZE + DSIZE;
  else
    asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

  /* Search the free list for a fit */
  if ((first_fit(asize)) != NULL) {
    place(bp, asize);
    return bp;
  }

  /* No fit found. Get more memory and place the block */
  extendsize = MAX(asize,CHUNKSIZE);
  if ((extend_heap(extendsize/WSIZE)) == NULL)
    return NULL;
  place(bp, asize);
  //return bp;
	//After finishing obtaining free chunk p, 
	//check heap correctness to catch bugs
 // if (debug) {
 // 	mm_checkheap(true);
 // }
  return (void *)bp;
} 

void *extend_heap(size_t words) 
{
    header_t *bp;
    size_t size;
    
    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
   // if ((int)(bp = mem_sbrk(size)) == -1) 
    //    return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* new epilogue header */

    /* Coalesce if the previous block was free */
    coalesce(bp);
    return bp;
}

void place(void *bp, size_t asize)
  /* $end mmplace-proto */
{
  size_t csize = GET_SIZE(HDRP(bp));   

  if ((csize - asize) >= (DSIZE + DSIZE)) { 
    PUT(HDRP(bp), PACK(asize, 1));
    PUT(FTRP(bp), PACK(asize, 1));
    bp = NEXT_BLKP(bp);
    PUT(HDRP(bp), PACK(csize-asize, 0));
    PUT(FTRP(bp), PACK(csize-asize, 0));
  }
  else { 
    PUT(HDRP(bp), PACK(csize, 1));
    PUT(FTRP(bp), PACK(csize, 1));
  }
}


/*
 * mm_free frees the previously allocated memory block
 */
void mm_free(void *ptr)
{
	size_t size = GET_SIZE(HDRP(ptr));
	PUT(HDRP(ptr), PACK(size, 0));
	PUT(FTRP(ptr), PACK(size, 0));
	coalesce(ptr);
}	

/*
 * mm_realloc changes the size of the memory block pointed to by ptr to size bytes.  
 * The contents will be unchanged in the range from the start of the region up to the minimum of   
 * the  old  and  new sizes.  If the new size is larger than the old size, the added memory will   
 * not be initialized.  If ptr is NULL, then the call is equivalent  to  malloc(size).
 * if size is equal to zero, and ptr is not NULL, then the call is equivalent to free(ptr).
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *old = ptr;
    void *new;
    size_t size2;
    
    new = mm_malloc(size); 
    if (new == NULL) {
    	mm_free (ptr); 
    	return NULL; 
    }
    else {

    	//(((size) + (ALIGNMENT-1)) & ~0x7)
	    size2 = *(size_t *)((char *)old - ((sizeof(size_t))+7 & -0x7));
	    if (size < size2) {
	      size2 = size;
	    }
	    memcpy(new, old, size2);
	    mm_free(old);
	    return new;
	}
}



 
heap_info_t mm_checkheap(bool verbose) 
{
	heap_info_t info;
	// Your code here
	// 
	// traverse the heap to fill in the fields of info

	// correctness of implicit heap amounts to the following assertion.
	assert(mem_heapsize() == (info.allocated_size + info.free_size));
	return info;

}

