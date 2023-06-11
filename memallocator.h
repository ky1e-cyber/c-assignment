#pragma once

#include <stddef.h>

void resetAllocator();

void* memalloc(size_t size);

void* memcalloc(size_t num, size_t size);

void* memrealloc(void* ptr, size_t newsize);

void memfree(void* ptr);

void freeHeap();

void printDebugAlloc();

void inspectHeap();
