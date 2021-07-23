#include "memory/VMM/VirtualAddressSpace.h"
namespace Memory{
    uint64_t VirtualAddressSpace::mappingForAddressIndex(uint64_t addr) const{
        for(uint64_t i = 0; i<mappings.getSize(); i++){
            if(mappings[i].containsAddress(addr))
                return i;
        }
    }
    void VirtualAddressSpace::map(const Mapping& mapping){
        uint64_t flags;
        if(mapping.getType() == Mapping::Type::LibData || 
        mapping.getType() == Mapping::Type::ProcessData ||
        mapping.getType() == Mapping::Type::ProcessStack) {
            flags = PT_Flag::ReadWrite;
        } else if(mapping.getType() == Mapping::Type::Device){
            flags = PT_Flag::ReadWrite | PT_Flag::WriteTrough;
        } else {
            flags = 0;
        }

        if(mapping.kernelMapping()){
            flags |= PT_Flag::UserSupper | PT_Flag::Global;
        }
        if(mapping.mappingType == Mapping::MappingType::All){
            for(uint64_t i = mapping.getStart(); i < mapping.getEnd(); i += 0x1000){
                vmmManager.MapMemory(i,PageFrameAllocator::getPMM().RequestPage(),flags);
            }
        } else if(mapping.mappingType == Mapping::MappingType::Physical){
            const DeviceMemoryMapping& devmapping = reinterpret_cast<const DeviceMemoryMapping&>(mapping);
            for(uint64_t i = 0; i < devmapping.getSize(); i++){
                vmmManager.MapMemory(devmapping.getStart() + i*0x1000, devmapping.getPhysicalStart() + i*0x1000,flags);
            }
        }
    }
    void VirtualAddressSpace::unmap(const Mapping& mapping){
        #ifdef DEBUG
        if(! mappings.contains(mapping))
            Util::Panic("Mapping not in mappings \n");
        #endif
        for(uint64_t i = mapping.getStart(); i < mapping.getEnd(); i += 0x1000){
            PageDirectoryEntry physicalMapping = vmmManager.UnmapMemory(i);
            if(physicalMapping.isValid() && mapping.mappingType != Mapping::MappingType::Physical){
                PageFrameAllocator::getPMM().FreePage(physicalMapping.GetAddress() << 12);
            }
        }
    }
    VirtualAddressSpace VirtualAddressSpace::newUserVAS(){
        PageTable* pf = reinterpret_cast<PageTable*>(PageFrameAllocator::getPMM().RequestPage());
        memset(pf,(uint8_t)0,0x1000);
        memcpy(&getKernelVAS().vmmManager.PML4Address->entries[PLM4KernelVASStartIndex], &pf->entries[PLM4KernelVASStartIndex], 512 - PLM4KernelVASStartIndex);
        return VirtualAddressSpace{VirtualMemoryManager{reinterpret_cast<PageTable*>(pf)}};

    }
}