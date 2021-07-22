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

        for(uint64_t i = mapping.getStart(); i < mapping.getEnd(); i += 0x1000){
            
        }
    }
    void VirtualAddressSpace::unmap(const Mapping& mapping){

    }
}