#pragma once
#include "../memory.h"
#include "PageStack.h"
#include "../../Util/Bitmap.h"
class PageFrameAllocator{
    public:
        PageFrameAllocator(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapSize, uint64_t mMapDescSize);
        uint64_t RequestPage();
        bool isInitialized()const;
    private:
        void FreePage(uint64_t addr);
        void LockPage(uint64_t addr);
        void ReservePage(uint64_t addr);
        uint64_t freeMemory = 0;
        uint64_t reservedMemory = 0;
        uint64_t usedMemory = 0;
        PageStack pageStack;
        Bitmap pageBitmap;
        bool InitializedSuccessfully = false;
};
extern PageFrameAllocator GlobalPageFrameAllocator;