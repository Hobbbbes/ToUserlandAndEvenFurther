#include "memory/Heap/Heap.h"
#include "memory/PMM/PageFrameAllocator.h"
#include "memory/VMM/VirtualMemoryManager.h"
#include "Util/panic.h"
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
constexpr uint64_t HeapEntryHeaderMagicValue = 0xe1efa87e1efa87ff;
Heap kernelHeap;
HeapEntryHeader* startSec;
HeapEntryHeader* head;


void initHeap(uint64_t begin, uint64_t end){
    kernelHeap = Heap(begin,end,end - begin);
    *reinterpret_cast<HeapEntryHeader*>(begin) = HeapEntryHeader(reinterpret_cast<HeapEntryHeader*>(begin)
    ,reinterpret_cast<HeapEntryHeader*>(begin),(end-begin) - sizeof(HeapEntryHeader));
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
            Util::Panic("Heap corruption detected");
        }
    }
    startSec = seg->nextHeader;
    return seg;
}
constexpr uint64_t MIN_HEAP_GROWTH_PAGES = 10;
bool growHeap(uint64_t to_grow){
    uint64_t pages = (to_grow / 0x1000) + 1;
    if (pages < MIN_HEAP_GROWTH_PAGES){
        pages = MIN_HEAP_GROWTH_PAGES;
    }
    for(uint64_t p = 0; p<pages; p++){
        if(KernelPMM.GetFreeRam() == 0){
            return false;
        }
        uint64_t pf = KernelPMM.RequestPage();
        KernelVMM.MapMemory(kernelHeap.end + (p * 0x1000), pf);
    }
    HeapEntryHeader* tail = head->previousHeader;
    if (reinterpret_cast<uint64_t>(tail) + tail->size + sizeof(HeapEntryHeader) == kernelHeap.end){
        tail->size += pages*0x1000;
    } else {
        HeapEntryHeader* newEntry = reinterpret_cast<HeapEntryHeader*>(kernelHeap.end);
        newEntry->size = pages*0x1000 - sizeof(HeapEntryHeader);
        newEntry->magicValue = HeapEntryHeaderMagicValue;
        newEntry->nextHeader = head;
        newEntry->previousHeader = tail;
        tail->nextHeader = newEntry;
        head->previousHeader = newEntry;
    }
    kernelHeap.end += pages*0x1000;
    kernelHeap.size += pages*0x1000;
    return true;
    
}
constexpr uint64_t HEAP_SHRINK = 10;
void shrinkHeap(){
    HeapEntryHeader* tail = head->previousHeader;
    if(tail->size >= HEAP_SHRINK*2*0x1000){
        tail->size -= HEAP_SHRINK * 0x1000;
        kernelHeap.size -= HEAP_SHRINK*0x1000;
        for(uint64_t p = 1; p <= HEAP_SHRINK; p++){
            uint64_t pf = KernelVMM.GetMapping(kernelHeap.end - (p*0x1000));
            KernelVMM.UnmapMemory(kernelHeap.end - (p*0x1000));
            KernelPMM.FreePage(pf);
        }
        kernelHeap.end -= HEAP_SHRINK*0x1000;
    }
}

void* operator new(size_t sz){
    if(sz%8 != 0){
        sz += 8 - (sz%8);//Blocks are 8 Byte aligned
    }
    HeapEntryHeader* seg = findFreeSegment(sz);
    while(seg == nullptr){
        bool suc = growHeap(sz);
        if(!suc){
            Util::Panic("Physical Memory exhausted");
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
        HeapEntryHeader* newHidden = reinterpret_cast<HeapEntryHeader*>(reinterpret_cast<uint64_t>(seg) + seg->size);
        seg->size -= sizeof(HeapEntryHeader);
        newHidden->magicValue = HeapEntryHeaderMagicValue;
        newHidden->size = sz;
        newHidden->nextHeader = nullptr;
        newHidden->previousHeader = nullptr;
        return reinterpret_cast<void*>(reinterpret_cast<uint64_t>(newHidden) + sizeof(HeapEntryHeader));
        //*newHidden = HeapEntryHeader(nullptr,)
    }
}
void* operator new[](size_t sz){
    return operator new(sz);
}
void operator delete(void* p,uint64_t sz){
    HeapEntryHeader* seg = reinterpret_cast<HeapEntryHeader*>(reinterpret_cast<uint64_t>(p) - sizeof(HeapEntryHeader));
    if(seg->magicValue != HeapEntryHeaderMagicValue){
        Util::Panic("Heap corruption detected");
        return;
    }
    HeapEntryHeader* node = head;
    while (reinterpret_cast<uint64_t>(node) < reinterpret_cast<uint64_t> (seg)){
        node = node->nextHeader;
        if (node == head){
            break;
        }
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
    HeapEntryHeader* tail = head->previousHeader;
    shrinkHeap();
}

void operator delete[](void* p){
    operator delete(p,0);
}