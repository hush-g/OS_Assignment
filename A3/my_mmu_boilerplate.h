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
    // Your implementation of my_free goes here
    struct memoryBlockHeader* curr = (struct memoryBlockHeader* )ptr;
    curr-=1;
    curr->free = true;
    combine();
}

void info() {
    // printf("\n");
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
        while(curr != NULL){
            fprintf(stderr, "Memory block of %zu size and occupied %s \n", curr -> size, !curr -> free ? "true":"false");
            curr = curr -> nextBlock;
        }
    }
}

// int main() {
//     // Using malloc

//     int* ptr1 = (int*)my_malloc(sizeof(int));
//     if (ptr1 == NULL) {
//         printf("Memory allocation failed.\n");
//         return 1;
//     }
//     info();
//     *ptr1 = 42;
//     printf("Using malloc: %d\n", *ptr1);
//     my_free(ptr1);
//     info();

//     // Using calloc
//     int* ptr2 = (int*)my_calloc(1, sizeof(int));
//     if (ptr2 == NULL) {
//         printf("Memory allocation failed.\n");
//         return 1;
//     }
//     info();
//     *ptr2 = 42;
//     printf("Using calloc: %d\n", *ptr2);
//     my_free(ptr2);
//     info();

//     // Using malloc again
//     int* ptr3 = (int*)my_malloc(sizeof(int));
//     if (ptr3 == NULL) {
//         printf("Memory allocation failed.\n");
//         return 1;
//     }
//     *ptr3 = 42;
//     info();
//     printf("Using malloc again: %d\n", *ptr3);
//     my_free(ptr3);
//     info();

//     return 0;
// }