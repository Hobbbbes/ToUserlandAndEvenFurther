#include "PageFrameAllocator.h"
PageFrameAllocator GlobalPageFrameAllocator;

PageFrameAllocator::PageFrameAllocator(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapSize, uint64_t mMapDescSize){
    if(InitializedSuccessfully) return;
    InitializedSuccessfully = true;
    uint64_t mMapEntries = mMapSize / mMapDescSize;
    uint64_t memorySize = GetMemorySize(mMap,mMapEntries,mMapDescSize);
    freeMemory = memorySize;

    void* largestFreeMemSeg = nullptr;
    uint64_t largestFreeMemSegSize = 0;
    for(uint64_t i = 0; i<mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = reinterpret_cast<EFI_MEMORY_DESCRIPTOR*>(reinterpret_cast<uint64_t>(mMap) + (i * mMapDescSize));
        if(desc->type == 7){ // type == EfiConventionalMemory
            if(desc->numPages * 4096 > largestFreeMemSegSize){
                largestFreeMemSeg = desc->physicalAddr;
                largestFreeMemSegSize = desc->numPages * 4096;
            } 
        }
    }
    uint64_t pageStackSize = (memorySize / 0x1000) * 8;
    if(largestFreeMemSegSize <= pageStackSize) return; //Real Problem
    pageStack = PageStack(reinterpret_cast<uint64_t*>(largestFreeMemSeg), reinterpret_cast<uint64_t*>(reinterpret_cast<uint64_t>(largestFreeMemSeg) + pageStackSize));

}