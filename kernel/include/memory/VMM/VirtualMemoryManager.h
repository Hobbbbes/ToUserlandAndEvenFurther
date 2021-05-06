#pragma once
#include "memory/VMM/PageTable.h"
class VirtualMemoryManager{
    public:
        VirtualMemoryManager(PageTable* PML4Addr);
        VirtualMemoryManager();
        PageDirectoryEntry* MapMemory(const uint64_t virtualAddr, const uint64_t physicalAddr, uint64_t flags = 0);
        void UnmapMemory(const uint64_t virtualAddr);
        PageDirectoryEntry GetMapping(const uint64_t virtualAddr);
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

extern VirtualMemoryManager KernelVMM;