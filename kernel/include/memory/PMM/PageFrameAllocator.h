#pragma once
#include "memory/memory.h"
#include "PageStack.h"
#include "Util/Bitmap.h"
namespace Memory{
class PageFrameAllocator{
    public:
        PageFrameAllocator(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapSize, uint64_t mMapDescSize);
        //PageFrameAllocator();
        uint64_t RequestPage();
        void FreePage(uint64_t addr);
        void FreePages(uint64_t addr, uint64_t count);
        void LockPages(uint64_t addr, uint64_t count);
        bool isInitialized()const;
        uint64_t GetFreeRam()const;
        uint64_t GetUsedRam()const;
        uint64_t GetReservedRam()const;
        static PageFrameAllocator& getPMM();
    private:
        void LockPage(uint64_t addr);

        void ReservePage(uint64_t addr);
        void ReservePages(uint64_t addr, uint64_t count);

        void UnreservePage(uint64_t addr);
        void UnreservePages(uint64_t addr, uint64_t count);
        
        uint64_t freeMemory = 0;
        uint64_t reservedMemory = 0;
        uint64_t usedMemory = 0;
        PageStack pageStack;
        Util::Bitmap pageBitmap;
        bool InitializedSuccessfully = false;
        static PageFrameAllocator KernelPMM;
};
}