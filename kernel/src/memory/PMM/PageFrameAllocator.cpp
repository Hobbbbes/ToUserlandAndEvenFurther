#include "memory/PMM/PageFrameAllocator.h"
#include "Util/panic.h"
namespace Memory{


PageFrameAllocator::PageFrameAllocator(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapSize, uint64_t mMapDescSize){
    
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
    if(largestFreeMemSegSize <= pageStackSize) {
        Util::Panic("Largest free memory segment is smaller than size required for PageStack");
    } //Real Problem
    pageStack = PageStack(reinterpret_cast<uint64_t*>(largestFreeMemSeg), reinterpret_cast<uint64_t*>(reinterpret_cast<uint64_t>(largestFreeMemSeg) + pageStackSize));
    uint64_t bitmapSize = memorySize / 4096 / 8 + 1;
    if(largestFreeMemSegSize - pageStackSize - 1 <= bitmapSize) {
        Util::Panic("Largest free memory segment is smaller than size required for Bitmap and PageStack");
    } //Real Problem
    pageBitmap = Util::Bitmap(bitmapSize,reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>(largestFreeMemSeg) + pageStackSize + 1));

    LockPages(reinterpret_cast<uint64_t>(largestFreeMemSeg), (1 + pageStackSize + bitmapSize) / 0x1000 + 1);

    for(uint64_t i = 0; i<mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = reinterpret_cast<EFI_MEMORY_DESCRIPTOR*>(reinterpret_cast<uint64_t>(mMap) + (i * mMapDescSize));
        if(desc->type != 7){
            ReservePages(reinterpret_cast<uint64_t>(desc->physicalAddr), desc->numPages);
        }
    }

    pageStack.InitPush(pageBitmap);
    InitializedSuccessfully = true;
    KernelPMM = *this;
}

//PageFrameAllocator::PageFrameAllocator(){}

PageFrameAllocator& PageFrameAllocator::getPMM(){
    #ifdef DEBUG
    if(!KernelPMM.isInitialized())
        Util::Panic("KernelPMM not initialized \n");
    #endif
    return KernelPMM;
}

void PageFrameAllocator::FreePage(uint64_t address){
    uint64_t index = address / 4096;
    if(pageBitmap[index] == false) return;
    if(pageBitmap.Set(index,false)){
        freeMemory += 4096;
        usedMemory -= 4096;
    }
    pageStack.Push(address);

}
void PageFrameAllocator::LockPage(uint64_t address){
    uint64_t index = address / 4096;
    if(pageBitmap[index] == true) return;
    if(pageBitmap.Set(index,true)){
        freeMemory -= 4096;
        usedMemory += 4096;
    }
}
void PageFrameAllocator::UnreservePage(uint64_t address){
    uint64_t index = reinterpret_cast<uint64_t>(address) / 4096;
    if(pageBitmap[index] == false) return;
    if(pageBitmap.Set(index,false)){
        freeMemory += 4096;
        reservedMemory -= 4096;
    }
    pageStack.Push(address);
}
void PageFrameAllocator::ReservePage(uint64_t address){
    uint64_t index = reinterpret_cast<uint64_t>(address) / 0x1000;
    if(pageBitmap[index] == true) return;
    if(pageBitmap.Set(index,true)){
        freeMemory -= 4096;
        reservedMemory += 4096;
    }
}

void PageFrameAllocator::FreePages(uint64_t address, uint64_t pageCount){
    for(uint64_t i = 0; i< pageCount; i++){
        FreePage(address + (i * 4096));
    }
}
void PageFrameAllocator::LockPages(uint64_t address, uint64_t pageCount){
    for(uint64_t i = 0; i< pageCount; i++){
        LockPage(address + (i * 4096));
    }
}

void PageFrameAllocator::UnreservePages(uint64_t address, uint64_t pageCount){
    for(uint64_t i = 0; i< pageCount; i++){
        UnreservePage(address + (i * 4096));
    }
}

void PageFrameAllocator::ReservePages(uint64_t address, uint64_t pageCount){
    for(uint64_t i = 0; i< pageCount; i++){
        ReservePage(address + (i * 0x1000));
    }
}

uint64_t PageFrameAllocator::GetFreeRam()const{
    return freeMemory;
}
uint64_t PageFrameAllocator::GetUsedRam()const{
    return usedMemory;
}
uint64_t PageFrameAllocator::GetReservedRam()const{
    return reservedMemory;
}

uint64_t PageFrameAllocator::RequestPage(){
    uint64_t addr;
    do{
        addr = pageStack.Pop();
    }while(pageBitmap[addr / 0x1000]); //As long as pages outputed by stack are in use
    pageBitmap.Set(addr / 0x1000, false);
    usedMemory += 0x1000;
    freeMemory -= 0x1000;
    return addr;
}
}