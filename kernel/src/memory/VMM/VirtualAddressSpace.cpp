#include "memory/VMM/VirtualAddressSpace.h"
namespace Memory{
    uint64_t VirtualAddressSpace::mappingForAddressIndex(uint64_t addr) const{
        for(uint64_t i = 0; i<mappings.getSize(); i++){
            if(mappings[i]->containsAddress(addr))
                return i;
        }
    }
    void VirtualAddressSpace::map(Util::UniquePtr<Mapping>&& mapping){
        uint64_t flags;
        if(mapping->getType() == Mapping::Type::LibData || 
        mapping->getType() == Mapping::Type::ProcessData ||
        mapping->getType() == Mapping::Type::ProcessStack) {
            flags = PT_Flag::ReadWrite;
        } else if(mapping->getType() == Mapping::Type::Device){
            flags = PT_Flag::ReadWrite | PT_Flag::WriteTrough;
        } else {
            flags = 0;
        }

        if(mapping->kernelMapping()){
            flags |= PT_Flag::UserSupper | PT_Flag::Global;
        }
        if(mapping->mappingType == Mapping::MappingType::All){
            for(uint64_t i = mapping->getStart(); i < mapping->getEnd(); i += 0x1000){
                vmm.MapMemory(i,PageFrameAllocator::getPMM().RequestPage(),flags);
            }
        } else if(mapping->mappingType == Mapping::MappingType::Physical){
            const DeviceMemoryMapping* devmapping = reinterpret_cast<DeviceMemoryMapping*>(mapping.getPtr());
            for(uint64_t i = 0; i < devmapping->getSize(); i++){
                vmm.MapMemory(devmapping->getStart() + i*0x1000, devmapping->getPhysicalStart() + i*0x1000,flags);
            }
        }
        mappings.push_back(mapping);
    }
    void VirtualAddressSpace::unmap(const Mapping& mapping){
        ASSERT(mappings.contains<Mapping>(mapping),"Mapping not in mappings \n")
        for(uint64_t i = mapping.getStart(); i < mapping.getEnd(); i += 0x1000){
            PageDirectoryEntry physicalMapping = vmm.UnmapMemory(i);
            if(physicalMapping.isValid() && mapping.mappingType != Mapping::MappingType::Physical){
                PageFrameAllocator::getPMM().FreePage(physicalMapping.GetAddress() << 12);
            }
        }
    }
    VirtualAddressSpace VirtualAddressSpace::newUserVAS(){
        PageTable* pf = reinterpret_cast<PageTable*>(PageFrameAllocator::getPMM().RequestPage());
        memset(pf,(uint8_t)0,0x1000);
        memcpy(&getKernelVAS().vmm.PML4Address->entries[PLM4KernelVASStartIndex], &pf->entries[PLM4KernelVASStartIndex], 512 - PLM4KernelVASStartIndex);
        return VirtualAddressSpace{VirtualMemoryManager{reinterpret_cast<PageTable*>(pf)}};
    }
    void Mapping::map(VirtualMemoryManager &vmm){
        ASSERT(type == Mapping::Type::Device || type == Mapping::Type::File || type == Mapping::Type::Physical || mappingType == Mapping::MappingType::Physical,
        "Mapping::map with File, Physical or Device Mapping")
        uint64_t flags;
        if(type == Mapping::Type::LibData || 
        type == Mapping::Type::ProcessData ||
        type == Mapping::Type::ProcessStack) {
            flags = PT_Flag::ReadWrite;
        } else {
            flags = 0;
        }
    }
    DeviceMemoryMapping(uint64_t vstart, uint64_t size, uint64_t physicalStart, bool kernel){
        
    }
    void DeviceMemoryMapping::map(VirtualMemoryManager &vmm) {

    }
}