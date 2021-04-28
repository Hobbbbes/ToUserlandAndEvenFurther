#include "memory/VMM/VirtualMemoryManager.h"
#include "memory/PMM/PageFrameAllocator.h"
#include "memory/memory.h"
VirtualMemoryManager KernelVMM;

VirtualMemoryManager::VirtualMemoryManager(PageTable* PML4Addr) : PML4Address(PML4Addr){}
VirtualMemoryManager::VirtualMemoryManager() : PML4Address(nullptr){}

PageDirectoryEntry* VirtualMemoryManager::MapMemory(const uint64_t virtAddr, const uint64_t physAddr){
    VirtualMemoryManager::PageStructureIndizes indizes = getIndizes(virtAddr);
    PageDirectoryEntry PDE;
    PDE = PML4Address->entries[indizes.PDP_i];
    PageTable* PDP;
    if(!PDE.GetFlag(PT_Flag::Present)){
        PDP = reinterpret_cast<PageTable*>(KernelPMM.RequestPage());
        memset(PDP,(uint8_t)0x00,0x1000);
        PDE.SetAddress(reinterpret_cast<uint64_t>(PDP) >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PML4Address->entries[indizes.PDP_i] = PDE;
    } else {
        PDP = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(PDE.GetAddress()) << 12);
    }
    PDE = PDP->entries[indizes.PD_i];
    PageTable* PD;
    if(!PDE.GetFlag(PT_Flag::Present)){
        PD = reinterpret_cast<PageTable*>(KernelPMM.RequestPage());
        memset(PD,(uint8_t)0x00,0x1000);
        PDE.SetAddress(reinterpret_cast<uint64_t>(PD) >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDP->entries[indizes.PD_i] = PDE;
    } else {
        PD = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(PDE.GetAddress()) << 12);
    }
    PDE = PD->entries[indizes.PT_i];
    PageTable* PT;
    if(!PDE.GetFlag(PT_Flag::Present)){
        PT = reinterpret_cast<PageTable*>(KernelPMM.RequestPage());
        memset(PT,(uint8_t)0x00,0x1000);
        PDE.SetAddress(reinterpret_cast<uint64_t>(PT) >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PD->entries[indizes.PT_i] = PDE;
    } else {
        PT = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(PDE.GetAddress()) << 12);
    }

    PDE = PT->entries[indizes.P_i];
    PDE.SetAddress(reinterpret_cast<uint64_t>(physAddr) >> 12);
    PDE.SetFlag(PT_Flag::Present,true);
    PDE.SetFlag(PT_Flag::ReadWrite, true);
    PT->entries[indizes.P_i] = PDE;
    return &PT->entries[indizes.P_i];
}

void VirtualMemoryManager::UnmapMemory(const uint64_t virtualAddr){
    VirtualMemoryManager::PageStructureIndizes indizes = getIndizes(virtualAddr);
    PageDirectoryEntry PDE;
    PDE = PML4Address->entries[indizes.PDP_i];
    PageTable* PDP;
    if(!PDE.GetFlag(PT_Flag::Present)){
        return;
    } else {
        PDP = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(PDE.GetAddress()) << 12);
    }
    PDE = PDP->entries[indizes.PD_i];
    PageTable* PD;
    if(!PDE.GetFlag(PT_Flag::Present)){
        return;
    } else {
        PD = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(PDE.GetAddress()) << 12);
    }
    PDE = PD->entries[indizes.PT_i];
    PageTable* PT;
    if(!PDE.GetFlag(PT_Flag::Present)){
        return;
    } else {
        PT = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(PDE.GetAddress()) << 12);
    }

    memset(&PT->entries[indizes.P_i],uint8_t(0),sizeof(PageDirectoryEntry));
    if(memcmp((void*)PT,(uint8_t)0,0x1000)){
        uint64_t pf = this->GetMapping(reinterpret_cast<uint64_t>(PT));
        KernelPMM.FreePage(pf);
        if(memcmp((void*)PD,(uint8_t)0,0x1000)){
            uint64_t pf = this->GetMapping(reinterpret_cast<uint64_t>(PD));
            KernelPMM.FreePage(pf);
            if(memcmp((void*)PDP,(uint8_t)0,0x1000)){
                uint64_t pf = this->GetMapping(reinterpret_cast<uint64_t>(PDP));
                KernelPMM.FreePage(pf);
            }
        }

    }


}
uint64_t VirtualMemoryManager::GetMapping(const uint64_t virtualAddr){
    VirtualMemoryManager::PageStructureIndizes indizes = getIndizes(virtualAddr);
    PageDirectoryEntry PDE;
    PDE = PML4Address->entries[indizes.PDP_i];
    PageTable* PDP;
    if(!PDE.GetFlag(PT_Flag::Present)){
        return -1;
    } else {
        PDP = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(PDE.GetAddress()) << 12);
    }
    PDE = PDP->entries[indizes.PD_i];
    PageTable* PD;
    if(!PDE.GetFlag(PT_Flag::Present)){
        return -1;
    } else {
        PD = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(PDE.GetAddress()) << 12);
    }
    PDE = PD->entries[indizes.PT_i];
    PageTable* PT;
    if(!PDE.GetFlag(PT_Flag::Present)){
        return -1;
    } else {
        PT = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(PDE.GetAddress()) << 12);
    }
    if(PDE.GetFlag(PT_Flag::Present)){
        return PT->entries[indizes.P_i].GetAddress();
    } else {
        return -1;
    }
}

VirtualMemoryManager::PageStructureIndizes VirtualMemoryManager::getIndizes(uint64_t virtualAddr){
    VirtualMemoryManager::PageStructureIndizes res;
    virtualAddr >>= 12;
    res.P_i = virtualAddr & 0x1ff;
    virtualAddr >>=9;
    res.PT_i = virtualAddr & 0x1ff;
    virtualAddr >>=9;
    res.PD_i = virtualAddr & 0x1ff;
    virtualAddr >>=9;
    res.PDP_i = virtualAddr & 0x1ff;
    return res;
}