#include "memory/VMM/VirtualAddressSpace.h"
#include <utility>
namespace Memory{
    VirtualAddressSpace VirtualAddressSpace::KernelVAS (VirtualMemoryManager(0));
    uint64_t VirtualAddressSpace::mappingForAddressIndex(uint64_t addr) const{
        for(uint64_t i = 0; i<mappings.getSize(); i++){
            if(mappings[i]->containsAddress(addr))
                return i;
        }
        return -1;
    }
    void VirtualAddressSpace::map(Util::UniquePtr<Mapping>&& mapping){
        mapping->map(vmm);
        mappings.push_back(Util::move(mapping));
    }
    void VirtualAddressSpace::unmap(const Mapping& mapping){
        ASSERT(mappings.contains<Mapping>(mapping),"Mapping not in mappings \n")
        mapping.unmap(vmm);
        mappings.remove<Mapping>(mapping);
    }
    VirtualAddressSpace VirtualAddressSpace::newUserVAS(){
        PageTable* pf = reinterpret_cast<PageTable*>(PageFrameAllocator::getPMM().RequestPage());
        memset(pf,(uint8_t)0,0x1000);
        memcpy(&getKernelVAS().vmm.PML4Address->entries[PLM4KernelVASStartIndex], &pf->entries[PLM4KernelVASStartIndex], 512 - PLM4KernelVASStartIndex);
        return VirtualAddressSpace{VirtualMemoryManager{reinterpret_cast<PageTable*>(pf)}};
    }

    Mapping::Mapping(uint64_t vstart, uint64_t size, Type type, MappingType mappingType, bool kernel)
     : start(vstart), size(size), type(type), mappingType(mappingType), kernel(kernel) {}

    void Mapping::map(VirtualMemoryManager &vmm) const {
        ASSERT(type == Mapping::Type::Device || type == Mapping::Type::File || mappingType == Mapping::MappingType::Physical,
        "Mapping::map with File, Physical or Device Mapping")
        uint64_t flags = 0;
        if(type == Mapping::Type::LibData || 
        type == Mapping::Type::ProcessData ||
        type == Mapping::Type::ProcessStack) {
            flags |= PT_Flag::ReadWrite;
        }
        if (kernel)
            flags |= PT_Flag::Global | PT_Flag::UserSupper;
        for(uint64_t i = 0; i < size; i++){
            vmm.MapMemory(start + (i*0x1000), PageFrameAllocator::getPMM().RequestPage(), flags);
        }
        const_cast<Mapping*>(this)->mapped = true;

    }

    void Mapping::unmap(VirtualMemoryManager &vmm) const {
        ASSERT(mapped, "Attempted to unmap not mapped Mapping\n")
        for(uint64_t i = 0; i<size;i++){
            PageDirectoryEntry pf = vmm.UnmapMemory(start + (i*0x1000));
            ASSERT(!pf.isValid(),"Invalid pf found when unmapping Mapping\n")
            PageFrameAllocator::getPMM().FreePage(pf.GetAddress() << 12);
        }
    }

    DeviceMapping::DeviceMapping(uint64_t vstart, uint64_t size, uint64_t physicalStart, bool kernel)
    : Mapping(vstart,size,Type::Device,MappingType::Physical,kernel),
    physicalStart(physicalStart){}

    void DeviceMapping::map(VirtualMemoryManager &vmm) const {
        ASSERT(type != Type::Device, "Device Mappings Type is not Type::Device\n")
        uint64_t flags = PT_Flag::CachedDisabled | PT_Flag::ReadWrite;
        if (kernel)
            flags |= PT_Flag::Global | PT_Flag::UserSupper;

        for(uint64_t i = 0; i < size; i++) {
            vmm.MapMemory(start + (i*0x1000), physicalStart + (i*0x1000),flags);
        } 
        const_cast<DeviceMapping*>(this)->mapped = true;
    }
    void DeviceMapping::unmap(VirtualMemoryManager &vmm) const {
        ASSERT(mapped, "Attempted to unmap not mapped Mapping\n")
        for(uint64_t i = 0; i<size;i++){
            PageDirectoryEntry pf = vmm.UnmapMemory(start + (i*0x1000));
            ASSERT(!pf.isValid(),"Invalid pf found when unmapping DeviceMapping\n")
        } 
    }
    PhysicalMapping::PhysicalMapping(uint64_t vstart,const Type type, const Util::vector<uint64_t>& physicalAddresses, bool kernel)
    : Mapping(vstart,physicalAddresses.getSize(),type,MappingType::Physical,kernel),
    physicalAddresses(physicalAddresses){}

    void PhysicalMapping::map(VirtualMemoryManager &vmm) const {
        ASSERT(mappingType != MappingType::Physical || type != Type::ProcessDataShared, "Physical Mapping without Physical MappingType \n")
        uint64_t flags = PT_Flag::ReadWrite;
        if (kernel)
            flags |= PT_Flag::Global | PT_Flag::UserSupper;
        for(uint64_t i = 0; i < size; i++){
            vmm.MapMemory(start + (i*0x1000), physicalAddresses[i], flags);
        }
        const_cast<PhysicalMapping*>(this)->mapped = true;
    }

    void PhysicalMapping::unmap(VirtualMemoryManager &vmm) const {
        ASSERT(mapped, "Attempted to unmap not mapped PhysicalMapping\n")
        for(uint64_t i = 0; i<size;i++){
            PageDirectoryEntry pf = vmm.UnmapMemory(start + (i*0x1000));
            ASSERT(!pf.isValid() || (pf.GetAddress() << 12) != physicalAddresses[i],"Invalid or changed pf found when unmapping PhysicalMapping\n")
        }
    }

    SpecialMapping::SpecialMapping(uint64_t vstart,uint64_t size, uint64_t flags)
    : Mapping(vstart,size,Type::Special, MappingType::All,true), flags(flags){}

    void SpecialMapping::map(VirtualMemoryManager &vmm) const {
        ASSERT(type != Type::Special,
        "SpecialMapping::map without special type")
        for(uint64_t i = 0; i < size; i++){
            vmm.MapMemory(start + (i*0x1000), PageFrameAllocator::getPMM().RequestPage(), flags);
        }
        const_cast<SpecialMapping*>(this)->mapped = true;
    }

    void SpecialMapping::unmap(VirtualMemoryManager &vmm) const {
        ASSERT(mapped, "Attempted to unmap not mapped SpecialMapping\n")
        for(uint64_t i = 0; i<size;i++){
            PageDirectoryEntry pf = vmm.UnmapMemory(start + (i*0x1000));
            ASSERT(!pf.isValid(),"Invalid pf found when unmapping SpecialMapping\n")
            PageFrameAllocator::getPMM().FreePage(pf.GetAddress() << 12);
        }
    }
}