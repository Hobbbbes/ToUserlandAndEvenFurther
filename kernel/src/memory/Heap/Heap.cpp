#include "Heap.h"
#include "../PMM/PageFrameAllocator.h"
#include "../VMM/VirtualMemoryManager.h"

constexpr uint64_t HeapEntryHeaderMagicValue = 0xe1efa87e1efa87;
Heap kernelHeap;
HeapEntryHeader* startSec;
HeapEntryHeader* head;


void initHeap(uint64_t begin, uint64_t end){
    kernelHeap = Heap(begin,end,end - begin);
    *reinterpret_cast<HeapEntryHeader*>(begin) = HeapEntryHeader(reinterpret_cast<HeapEntryHeader*>(begin),reinterpret_cast<HeapEntryHeader*>(begin),(end-begin) - sizeof(HeapEntryHeader));
    startSec = reinterpret_cast<HeapEntryHeader*>(begin);
    head = startSec;
}

Heap::Heap(uint64_t b, uint64_t e, uint64_t sz)
: begin(b), end(e), size(sz){}
Heap::Heap()
: begin(0),end(0),size(0) {}

HeapEntryHeader::HeapEntryHeader(HeapEntryHeader* next, HeapEntryHeader* prev, uint64_t sz)
: nextHeader(next), previousHeader(prev), size(sz){}

//Implementation of "Next Fit" Called Algorithm

HeapEntryHeader* findFreeSegment(size_t sz){
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
    return false;
}

void* operator new(size_t sz){
    sz += 8 - (sz%8);//Blocks are 8 Byte aligned
    HeapEntryHeader* seg = findFreeSegment(sz);
    while(seg == nullptr){
        bool suc = growHeap();
        if(!suc){
            //TODO Panic (optional)
            //return nullptr;
        }
        seg = findFreeSegment(sz);
    }
    seg->size -= sz;
    if(seg->size < sizeof(HeapEntryHeader) * 2){
        seg->size += sz;
        seg->previousHeader->nextHeader = seg->nextHeader;
        seg->nextHeader->previousHeader = seg->previousHeader;
        seg->nextHeader = nullptr;
        seg->previousHeader = nullptr;
        return reinterpret_cast<void*>(reinterpret_cast<uint64_t>(seg + sizeof(HeapEntryHeader)));
    } else {
        seg->size -= sizeof(HeapEntryHeader);
        HeapEntryHeader* newHidden = reinterpret_cast<HeapEntryHeader*>(reinterpret_cast<uint64_t>(seg) + seg->size);
        newHidden->magicValue = HeapEntryHeaderMagicValue;
        newHidden->size = sz;
        newHidden->nextHeader = nullptr;
        newHidden->previousHeader = nullptr;
        return reinterpret_cast<void*>(reinterpret_cast<uint64_t>(newHidden + sizeof(HeapEntryHeader)));
        //*newHidden = HeapEntryHeader(nullptr,)
    }
}
void* operator new[](size_t sz){
    return operator new(sz);
}
void operator delete(void* p){
    HeapEntryHeader* seg = reinterpret_cast<HeapEntryHeader*>(reinterpret_cast<uint64_t>(p) - sizeof(HeapEntryHeader));
    if(seg->magicValue != HeapEntryHeaderMagicValue){
        //TODO: Panic
        return;
    }
    HeapEntryHeader* node = head;
    while (reinterpret_cast<uint64_t>(node) < reinterpret_cast<uint64_t> (seg)){
        node = node->nextHeader;
    }
    HeapEntryHeader* prev = node->previousHeader;
    //Check for unification
    bool insert = true;
    if(reinterpret_cast<uint64_t>(prev) + prev->size + sizeof(HeapEntryHeader) == reinterpret_cast<uint64_t>(seg)){
        prev->size += seg->size + sizeof(HeapEntryHeader);
        memset((void*)seg,(uint8_t)0,sizeof(HeapEntryHeader));
        seg = prev;
        insert = false;
    }
    if (reinterpret_cast<uint64_t>(seg) + seg->size + sizeof(HeapEntryHeader) == reinterpret_cast<uint64_t>(node)){
        seg->size += node->size + sizeof(HeapEntryHeader);
        seg->nextHeader = node->nextHeader;
        node->nextHeader->previousHeader = seg;
        memset((void*)node,(uint8_t)0,sizeof(HeapEntryHeader));
        insert = false;
    }
    //Insert freed block
    if(insert){
        prev->nextHeader = seg;
        node->previousHeader = seg;
        seg->nextHeader = node;
        seg->previousHeader = prev;
    }
}

void operator delete[](void* p){
    operator delete(p);
}