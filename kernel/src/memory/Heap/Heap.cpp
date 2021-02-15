#include "Heap.h"
#include "../PMM/PageFrameAllocator.h"
#include "../VMM/VirtualMemoryManager.h"
/*
struct Heap{
    uint64_t begin;
    uint64_t end;
    Heap(uint64_t b, uint64_t e);
    Heap();
};

//Used for Free List
struct HeapEntryHeader{
    HeapEntryHeader* nextHeader;
    HeapEntryHeader* previousHeader;
};

void* operator new(size_t sz);
void operator delete(void* ptr);
*/
constexpr uint64_t HeapEntryHeaderMagicValue = 0xe1efa87e1efa87;
Heap kernelHeap;

void initHeap(uint64_t begin, uint64_t end){
    kernelHeap = Heap(begin,end,end - begin);
    *reinterpret_cast<HeapEntryHeader*>(begin) = HeapEntryHeader(nullptr,nullptr,(end-begin) - sizeof(HeapEntryHeader));
}

HeapEntryHeader::HeapEntryHeader(HeapEntryHeader* next, HeapEntryHeader* prev, uint64_t sz)
: nextHeader(next), previousHeader(prev), size(sz){}

//Implementation of "Next Fit" Called Algorithm
HeapEntryHeader* findFreeSegment(size_t sz){
    static HeapEntryHeader* startSec;
    HeapEntryHeader* seg = startSec;
    while (seg->size < sz){
        seg = seg->nextHeader;
        if(seg == startSec){
            return nullptr;
        } else if(seg->magicValue != HeapEntryHeaderMagicValue){
            //TODO Panic
        }
    }
    startSec = seg->nextHeader;
    return seg;
}

bool growHeap(){

}

void* operator new(size_t sz){
    sz += 8 - (sz%8);//Blocks are 8 Byte aligned
    HeapEntryHeader* seg = findFreeSegment(sz);
    if(seg == nullptr){
        bool suc = growHeap();
        if(!suc){
            //TODO Panic (optional)
            return nullptr;
        }
        seg = findFreeSegment(sz);
    }
    seg->size -= sz;
    if(seg->size < sizeof(HeapEntryHeader) * 2){
        seg->size += sz;
        seg->previousHeader->nextHeader = seg->nextHeader;
        seg->nextHeader->previousHeader = seg->previousHeader;
        return reinterpret_cast<void*>(reinterpret_cast<uint64_t>(seg + sizeof(HeapEntryHeader)));
    } else {
        seg->size -= sizeof(HeapEntryHeader);
        HeapEntryHeader* newHidden = reinterpret_cast<HeapEntryHeader*>(reinterpret_cast<uint64_t>(seg) + seg->size);
        //*newHidden = HeapEntryHeader(nullptr,)
    }



}