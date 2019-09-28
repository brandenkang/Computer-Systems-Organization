#ifndef MM_COMMON_H
#define MM_COMMON_H

#include <stdio.h>
#include <stdbool.h>
typedef struct {
	size_t num_allocated_chunks; //number of allocated chunks
	size_t num_free_chunks; // number of free chunks
	size_t allocated_size; //sum of sizes of all allocated chunks
	size_t free_size; //sum of sizes of all free chunks
} heap_info_t;

int mm_init (void);
void *mm_malloc (size_t size);
void mm_free (void *ptr);
void *mm_realloc(void *ptr, size_t size);
heap_info_t mm_checkheap(bool verbose);



#define WSIZE      				8
#define DSIZE      				16
#define CHUNKSIZE  				(1<<10)  /* Extend heap by this amount (bytes) */ 
#define BLK_SIZE(size) 			(((size)<=2*DSIZE) ? MIN_BLK_SIZE : ALIGN(DSIZE+size))
#define MIN_BLK_SIZE 			(3*DSIZE)

#define ALIGN(p) (((size_t)(p) + ((7) & ~0x7)
#define MAX(x, y) 				((x) > (y) ? (x) : (y))
#define MIN(x, y) 				((x) < (y) ? (x) : (y))  
#define PACK(size, alloc)  		((size) | (alloc))
#define GET(p)       			(*(unsigned int *)(p))
#define PUT(p, val)  			(*(unsigned int *)(p) = (val))
#define GET_SIZE(p)   			(GET(p) & ~0x7)
#define GET_ALLOC(p)  			(GET(p) & 0x1)

#define HDRP(bp)  				((char *)(bp) - WSIZE)
#define FTRP(bp)  				((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
#define NEXT_BLKP(bp)  			((header_t *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp)  			((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))
#define GET_NEXT(bp)  			(*(char **)(bp + WSIZE))
#define GET_PREV(bp)  			(*(char **)(bp))
#define SET_NEXT(bp, new_bp) 	(GET_NEXT(bp) = new_bp)
#define SET_PREV(bp, new_bp) 	(GET_PREV(bp) = new_bp)


#endif
