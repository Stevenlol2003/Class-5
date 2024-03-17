#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "p3Heap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct blockHeader {

    int size_status;

    /*
     * Size of the block is always a multiple of 8.
     * Size is stored in all block headers and in free block footers.
     *
     * Status is stored only in headers using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     *
     * Start Heap:
     *  The blockHeader for the first block of the heap is after skip 4 bytes.
     *  This ensures alignment requirements can be met.
     *
     * End Mark:
     *  The end of the available memory is indicated using a size_status of 1.
     *
     * Examples:
     *
     * 1. Allocated block of size 24 bytes:
     *    Allocated Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 25
     *      If the previous block is allocated p-bit=1 size_status would be 27
     *
     * 2. Free block of size 24 bytes:
     *    Free Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 24
     *      If the previous block is allocated p-bit=1 size_status would be 26
     *    Free Block Footer:
     *      size_status should be 24
     */
} blockHeader;

/* Global variable - DO NOT CHANGE NAME or TYPE.
 * It must point to the first block in the heap and is set by init_heap()
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;

/* Size of heap allocation padded to round to nearest page size.
 */
int alloc_size;

/*
 * Additional global variables may be added as needed below
 * TODO: add global variables needed by your function
 */

/*
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if size < 1
 * - Determine block size rounding up to a multiple of 8
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements
 *       - Update all heap block header(s) and footer(s)
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 *   Return if NULL unable to find and allocate block for required size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* balloc(int size) {
	if (size < 1) {
		return NULL;
	}

	int block_size = sizeof(blockHeader) + size;

	// calculate padding need to make block_size a multiple of 8
	if (block_size % 8 != 0) {
		int padding = 8 - (sizeof(blockHeader) + size) % 8;
		block_size = block_size + padding;
	}

	// find the best-fit free block
	blockHeader* best_fit = NULL;
	blockHeader* current = heap_start;

	// end mark was set to 1, search through the heap until a blockHeader with size_status 1 is reached
	while (current -> size_status != 1) {
		// if allocated, skip and go to next block
        	if (current -> size_status % 2 == 1) {
            	current = (blockHeader*)((void*)current + (current -> size_status >> 2) * sizeof(blockHeader));
        		continue;
		}

		// if a perfect fit is found, break
		if ((current -> size_status >> 2) * sizeof(blockHeader) == block_size) {
			best_fit = current;
			break;
		}

		// check if current block is free and has size greater or equal to the needed block size
		if ((current -> size_status & 1) == 0 && ((current -> size_status >> 2) * sizeof(blockHeader)) >= block_size) {
			// if current block has less extra free space, make it the new best_fit block
			if (best_fit == NULL || (current -> size_status >> 2) < (best_fit -> size_status >> 2)) {
				best_fit = current;
			}
		}
		
		// go to next block
		int current_size = (current -> size_status >> 2) * sizeof(blockHeader);
		current = (blockHeader*)((char*)current + current_size);
	}

	// if no best-fit block was found, return NULL
	if (best_fit == NULL) {
		return NULL;
	}

	// if the best-fit block is exact size match
	if ((best_fit -> size_status >> 2) * sizeof(blockHeader) == block_size) {
		// mark the block as allocated
		best_fit -> size_status = best_fit -> size_status | 1;

		// mark the next block's p-bit as allocated
		blockHeader *next_block = (blockHeader*)((char*)best_fit + block_size);
		if (next_block -> size_status != 1) {
			next_block -> size_status = next_block -> size_status | 2;
		}

		return (void*)((char*)best_fit + sizeof(blockHeader));
	}

	// if no block is exact size, but there are larger blocks, split block
	if ((best_fit -> size_status >> 2) * sizeof(blockHeader) > block_size) {
		int free_block_size = (best_fit -> size_status >> 2) * sizeof(blockHeader) - block_size;

		//split the block into an allocated block and a free block
		blockHeader *allocated_block = best_fit;

		// mark used block as allocated
		allocated_block -> size_status = block_size | 1;

		blockHeader *free_block = (blockHeader*)((char*)allocated_block + block_size);
		free_block -> size_status = free_block_size;
		// update the free block's p-bit
		free_block -> size_status = free_block -> size_status | 2;

		return (void*)((char*)allocated_block + sizeof(blockHeader));
    }

        return NULL;
}

/*
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 */
int bfree(void *ptr) {
	// check if ptr is NULL
	if (ptr == NULL) {
		return -1;
	}
	
	// go from ptr address to header address
	blockHeader* current = (blockHeader*)(ptr - sizeof(blockHeader));

	// block size of current block
	int block_size = (current -> size_status >> 2) * sizeof(blockHeader);

	// check if ptr is a multiple of 8
	if (block_size % 8 != 0) {
		return -1;
	}

	// check if ptr is outside of the heap space
	if (current < heap_start || current > (blockHeader*)((void*)heap_start + alloc_size)) {
		return -1;
	}

	// check if block is already freed
	if ((current -> size_status % 2) == 0) {
		return -1;
	}

	// mark the block as unallocated by changing a-bit to 0
	current -> size_status = current -> size_status - 1;

	// change next block's p-bit to 0
	blockHeader* next_block = (blockHeader*)((void*)current + block_size);
	if (next_block -> size_status != 1) {
		next_block -> size_status = next_block -> size_status & ~2;
	}

	// no immediate coalescing
	return 0;
}

/*
 * Function for traversing heap block list and coalescing all adjacent
 * free blocks.
 *
 * This function is used for user-called coalescing.
 * Updated header size_status and footer size_status as needed.
 */
int coalesce() { 
	blockHeader* current = heap_start;

	// end mark was set to 1, search through the heap until a blockHeader with size_status 1 is reached 
	while (current -> size_status != 1) { 
		// if allocated, skip and go to next block 
		if (current -> size_status % 2 == 1) { 
			current = (blockHeader*)((void*)current + (current -> size_status >> 2) * sizeof(blockHeader));
		} 
		// else current block is free, check if next block is free 
		else { 
			// save the next block 
			blockHeader* next_block = (blockHeader*)((void*)current + (current -> size_status >> 2) * sizeof(blockHeader));

			// keep coalescing if the next_block is free 
			while (next_block -> size_status % 2 == 0) { 
				// update current block's size 
				current -> size_status = current -> size_status + (next_block -> size_status >> 2) * sizeof(blockHeader);

				// update next_block 
				next_block = (blockHeader*)((void*)current + (current -> size_status >> 2) * sizeof(blockHeader));
			} 

			// update next next block's p-bit to 0 if it's not end_mark 
			if (next_block -> size_status != 1) { 
				next_block -> size_status = next_block -> size_status & ~2;
			} 

			// go to next block 
			current = (blockHeader*)((void*)current + (current -> size_status >> 2) * sizeof(blockHeader));
		} 

	}

	return 0;
}


/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */
int init_heap(int sizeOfRegion) {

    static int allocated_once = 0; //prevent multiple myInit calls

    int   pagesize; // page size
    int   padsize;  // size of padding when heap size not a multiple of page size
    void* mmap_ptr; // pointer to memory mapped area
    int   fd;

    blockHeader* end_mark;

    if (0 != allocated_once) {
        fprintf(stderr,
        "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }

    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize from O.S.
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }

    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heap_start = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    end_mark = (blockHeader*)((void*)heap_start + alloc_size);
    end_mark->size_status = 1;

    // Set size in header
    heap_start->size_status = alloc_size;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heap_start->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((void*)heap_start + alloc_size - 4);
    footer->size_status = alloc_size;

    return 0;
}

/*
 * Function can be used for DEBUGGING to help you visualize your heap structure.
 * Traverses heap blocks and prints info about each block found.
 *
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts)
 * t_End    : address of the last byte in the block
 * t_Size   : size of the block as stored in the block header
 */
void disp_heap() {

    int    counter;
    char   status[6];
    char   p_status[6];
    char * t_begin = NULL;
    char * t_end   = NULL;
    int    t_size;

    blockHeader *current = heap_start;
    counter = 1;

    int used_size =  0;
    int free_size =  0;
    int is_used   = -1;

    fprintf(stdout,
        "*********************************** HEAP: Block List ****************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout,
        "---------------------------------------------------------------------------------\n");

    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;

        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "alloc");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "FREE ");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "alloc");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "FREE ");
        }

        if (is_used)
            used_size += t_size;
        else
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status,
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout,
        "---------------------------------------------------------------------------------\n");
    fprintf(stdout,
        "*********************************************************************************\n");
    fprintf(stdout, "Total used size = %4d\n", used_size);
    fprintf(stdout, "Total free size = %4d\n", free_size);
    fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
    fprintf(stdout,
        "*********************************************************************************\n");
    fflush(stdout);

    return;
}