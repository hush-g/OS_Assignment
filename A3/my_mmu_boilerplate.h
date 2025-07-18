#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

// Include your Headers below
#include <string.h>
#include <stdbool.h>

// You are not allowed to use the function malloc and calloc directly .

// freeMemoryList 
struct memoryBlockHeader {
    size_t size;
    bool free;
    struct memoryBlockHeader *nextBlock;
};

struct memoryBlockHeader *freeListHead = NULL;

void breakMemoryBlock (struct memoryBlockHeader *prevBlock, size_t size){
    struct memoryBlockHeader *newBlock;
    newBlock = (struct memoryBlockHeader*)((char*)prevBlock + sizeof(struct memoryBlockHeader) + size);
    newBlock -> free = true;
    newBlock -> size = prevBlock -> size - size - sizeof(struct memoryBlockHeader);
    newBlock -> nextBlock = prevBlock -> nextBlock;
    prevBlock -> size = size;
    prevBlock -> nextBlock = newBlock;
}

// Function to allocate memory using mmap
void* my_malloc(size_t size) {
    // Your implementation of my_malloc goes here
    if (size <= 0) {
      return NULL;
    }

    struct memoryBlockHeader *curr; 

    // The process doesn't have any memory allocated for it.
    if(freeListHead == NULL){
        void* addr  = mmap(NULL, size + sizeof(struct memoryBlockHeader), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if(addr == MAP_FAILED){
            perror("mmap");
            return NULL;
        }

        curr = (struct memoryBlockHeader *)addr;
        curr->size = size;
        curr->free = false;
        curr->nextBlock = NULL;
        freeListHead = curr;
    }else{
        curr = freeListHead;

        while((curr->size < size || !curr->free) && curr->nextBlock != NULL){
            curr = curr -> nextBlock;
        }

        // The first block that fits our criteria 
        if (curr -> free && curr->size >= size){
            // If the size of the free block is more than the 
            if (curr -> size > size + sizeof(struct memoryBlockHeader)){
                breakMemoryBlock(curr, size);
            }
            curr -> free = false;
        }else{
            void* addr  = mmap(NULL, size + sizeof(struct memoryBlockHeader), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            if(addr == MAP_FAILED){
                perror("mmap");
                return NULL;
            }

            curr -> nextBlock = (struct memoryBlockHeader *)addr;

            curr = curr ->nextBlock;
            curr->size = size;
            curr->free = false;
            curr->nextBlock = NULL;
        }
    }

    return (void *)(curr + 1);
}

// Function to allocate and initialize memory to zero using mmap
void* my_calloc(size_t nelem, size_t size) {
    // Your implementation of my_calloc goes here
    if (nelem*size <= 0) {
      return NULL;
    }

    size = size*nelem;

    void * curr = my_malloc(size);
    memset(curr, 0, size);

    return curr;
}

void combine(){
    struct memoryBlockHeader *curr;
    curr = freeListHead;
    while(curr != NULL && curr->nextBlock != NULL){
        if((curr->free) && (curr->nextBlock->free)){
            curr->size += (curr->nextBlock->size)+sizeof(struct memoryBlockHeader);
            curr->nextBlock=curr->nextBlock->nextBlock;
        }
        curr=curr->nextBlock;
    }
}

// Function to release memory allocated using my_malloc and my_calloc
void my_free(void* ptr) {
    if (ptr == NULL){
        return;
    }
    // Your implementation of my_free goes here
    struct memoryBlockHeader* curr = (struct memoryBlockHeader* )ptr;
    curr-=1;
    curr->free = true;
    ptr = NULL;
    combine();
}

void info() {
    size_t total = 0;
    struct memoryBlockHeader * curr = freeListHead;
    while( curr != NULL) {
        total += curr->size;
        curr = curr -> nextBlock;
    }
    fprintf(stderr,"The total size occupied by heap is: %zu \n", total);
    curr = freeListHead;
    if (curr != NULL){
        fprintf(stderr, "The structure of the free list of heap is: \n");
        int i = 1;
        while(curr != NULL){
            fprintf(stderr, "Memory block no. %d of %zu size and occupied: %s \n", i,  curr -> size, !curr -> free ? "true":"false");
            curr = curr -> nextBlock;
            i++;
        }
    }
}