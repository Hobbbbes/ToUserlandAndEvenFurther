#pragma once
#include "PageTable.h"
class VirtualMemoryManager{
    public:
        VirtualMemoryManager(PageTable* PML4Addr);
        VirtualMemoryManager();
        void MapMemory(const uint64_t virtualAddr, const uint64_t physicalAddr);
    private:
        PageTable* PML4Address;
        struct PageStructureIndizes{
            uint64_t PDP_i;
            uint64_t PD_i;
            uint64_t PT_i;
            uint64_t P_i;
        };
        
        PageStructureIndizes getIndizes(uint64_t virtualAddr);
};