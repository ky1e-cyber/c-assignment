#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "nullCheck.h"
#include "memallocator_options.h"

typedef unsigned char byte;

#define IS_PTR_IN_HEAP(ptr) \
    (((void*)(ptr) >= heap) && ((void*)(ptr) <= lastBlock))


void* heap = NULL;
void* lastBlock = NULL;

typedef struct {
    byte* head;
    byte* leftEdge;
    byte* rightEdge;
    size_t partSize;
} heapChunk;

heapChunk chunksArr[MEMALLOC_CHUNKS_COUNT];

void resetAllocator() {
    free(heap);

    const size_t chunkSize = MEMALLOC_MAXSIZE / MEMALLOC_CHUNKS_COUNT;

    assert(sizeof(void*) <= MEMALLOC_BASE_SIZE);
    assert(MEMALLOC_MAXSIZE % MEMALLOC_BASE_SIZE == 0);
    assert( ((MEMALLOC_MAXSIZE % MEMALLOC_CHUNKS_COUNT) == 0) && ((chunkSize % MEMALLOC_BASE_SIZE) == 0) );

    heap = malloc(MEMALLOC_MAXSIZE);
    nullCheck(heap);

    byte* currPtr = (byte* )heap;
    size_t chunkPartSize = MEMALLOC_BASE_SIZE;

    for (size_t i = 0; i < MEMALLOC_CHUNKS_COUNT; i++) {
        chunksArr[i].head = currPtr;
        chunksArr[i].leftEdge = currPtr;
        chunksArr[i].partSize = chunkPartSize; 

        const size_t partsCount = chunkSize / chunkPartSize;

        for (size_t j = 0; j < (partsCount - 1); j++) {
            lastBlock = currPtr;
            *((byte**)currPtr) = currPtr + chunkPartSize;
            currPtr += chunkPartSize;
        }
        *((byte**)currPtr) = NULL;
        chunksArr[i].rightEdge = currPtr;
        currPtr += chunkPartSize;

        chunkPartSize *= 2;
    }
}

byte* allocBlock(heapChunk* chunk) {
    if (chunk->head == NULL) 
        return NULL;

    byte* block = chunk->head;

    chunk->head = *((byte**)block);
    return block;
}

void insertBlock(heapChunk* chunk, byte* block) {
    if (!IS_PTR_IN_HEAP(block)) 
        return;

    *((byte**)block) = chunk->head;
    chunk->head = block;
}

heapChunk* findChunk(byte* block) {
    assert(IS_PTR_IN_HEAP(block));

    for (size_t i = 0; i < MEMALLOC_CHUNKS_COUNT; i++) {
        if ((block >= chunksArr[i].leftEdge) && (block <= chunksArr[i].rightEdge)) 
            return chunksArr + i;
    }

    return NULL;
}

void* memalloc(size_t size) {
    heapChunk* chunk = chunksArr;

    size_t i = 0;
    for ( ; (i < MEMALLOC_CHUNKS_COUNT) && ((chunksArr + i)->partSize < size); i++) ;

    if (i >= MEMALLOC_CHUNKS_COUNT) 
        return NULL;

    return (void* )allocBlock(chunksArr + i);
}

void* memcalloc(size_t num, size_t size) {
    void* ptr = memalloc(num * size);
    if (ptr == NULL) 
        return NULL;
    return memset(ptr, 0, num * size);
}

void memfree(void* ptr) {
    if (!IS_PTR_IN_HEAP(ptr)) 
        return;

    heapChunk* chunk = findChunk((byte* )ptr);
    if (chunk == NULL) 
        return;

    insertBlock(chunk, (byte* )ptr);
}

void* memrealloc(void* ptr, size_t newsize) {
    if (!IS_PTR_IN_HEAP(ptr))
        return NULL;

    heapChunk* chunk = findChunk(ptr);

    _Bool isNewsizeBigger = newsize > chunk->partSize;
    _Bool isInCurrentChunk = (chunk->partSize == MEMALLOC_BASE_SIZE) || (newsize > (chunk->partSize / 2));

    if ((!isNewsizeBigger) && isInCurrentChunk) 
        return ptr;

    void* newPtr = memalloc(newsize);

    if (newPtr == NULL) {
        if (isNewsizeBigger) {
            memfree(ptr);
            return newPtr;
        } else {
            return ptr;
        }
    }
    
    memcpy(newPtr, ptr, isNewsizeBigger ? chunk->partSize : newsize);
    memfree(ptr);
    return newPtr;
}

void freeHeap() {
    free(heap);
}