#pragma once
#include <stdint.h>
#include <stddef.h>

void* operator new[](size_t sz);
void* operator new(size_t sz);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void initHeap(uint64_t begin, uint64_t end);