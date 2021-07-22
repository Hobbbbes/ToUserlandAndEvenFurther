#include "memory/VMM/VirtualAddressSpace.h"
namespace Memory{
    uint64_t VirtualAddressSpace::mappingForAddressIndex(uint64_t addr) const{
        for(uint64_t i = 0; i<mappings.getSize(); i++){
            if(mappings[i].containsAddress(addr))
                return i;
        }
    }
}