#pragma once
#include <stdint.h>
#include <stddef.h>
struct Heap{
    uint64_t begin;
    uint64_t end;
    uint64_t size;
    Heap(uint64_t b, uint64_t e, uint64_t sz);
    Heap();
};


//Used for Free List
struct HeapEntryHeader{
    uint64_t magicValue = 0xe1efa87e1efa87;
    uint64_t size;
    HeapEntryHeader* nextHeader;
    HeapEntryHeader* previousHeader;
    HeapEntryHeader(HeapEntryHeader* next, HeapEntryHeader* prev, uint64_t sz);
};

void* operator new[](size_t sz);
void* operator new(size_t sz);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void initHeap(uint64_t begin, uint64_t end);