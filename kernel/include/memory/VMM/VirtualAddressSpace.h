#pragma once
#include <stdint.h>
#include "memory/VMM/VirtualMemoryManager.h"
#include "Util/vector.h"
#include "memory/memory.h"
namespace Memory{
    
    class Mapping{
        public:
            enum class Type : uint8_t {
                ProcessCode,
                ProcessData,
                ProcessDataShared,
                ProcessStack,
                LibCode,
                LibData,
                File,
                Device,
                Shared
            };
            enum class MappingType : uint8_t{
                All,
                OnDemand,
                Physical
            };
            Mapping(uint64_t start, uint64_t size, Type type, MappingType mappingType, bool kernel = false);
            inline uint64_t getStart() const {return start;}
            inline uint64_t getEnd() const {return start + (size*0x1000) - 1;}
            inline Type getType() const {return type;}
            inline uint64_t getSize() const {return size;}
            inline bool kernelMapping() const {return kernel;}
            inline bool containsAddress(uint64_t addr) const {return addr >= getStart() && addr <= getEnd();}
            inline bool operator == (const Mapping &other) const {return other.start == start && other.size == size && other.type == type;}
            inline bool operator != (const Mapping &other) const {return ! (other == *this);}
            virtual void map(VirtualMemoryManager& vmm);
        private:
            uint64_t start;
            uint64_t size;
            Type type;
            MappingType mappingType;
            bool kernel;
        friend VirtualAddressSpace;
    };
    class DeviceMemoryMapping : public Mapping {
        public:
            DeviceMemoryMapping(uint64_t start, uint64_t size, uint64_t physicalStart, bool kernel = false);
            inline uint64_t getPhysicalStart() const {return physicalStart;}
            void map(VirtualMemoryManager &vmm) override;
        private:
            uint64_t physicalStart;
    };

    class FileMemoryMapping : public Mapping {
        public:
            FileMemoryMapping(uint64_t start, uint64_t size, bool kernel = false);
            //map file somehow
        private:
            //File Handel
    };
    /*
    class SharedMemoryMapping : public Mapping {
        public:

    }
    */
    class VirtualAddressSpace{
        public:
            inline VirtualAddressSpace(VirtualMemoryManager& vmmManager) : vmm(vmmManager){}
            inline VirtualAddressSpace(VirtualMemoryManager vmmManager) : vmm(vmmManager){}
            uint64_t mappingForAddressIndex(uint64_t addr) const;
            inline const Mapping& mappingByIndex(uint64_t index) const {return *mappings[index];}
            inline const Mapping& mappingForAddress(uint64_t addr) const {
                return mappingByIndex(mappingForAddressIndex(addr));
            }
            void map(Util::UniquePtr<Mapping>&& mapping);
            void unmap(const Mapping& mapping);
            static VirtualAddressSpace newUserVAS();
            inline static VirtualAddressSpace& getKernelVAS() {return VirtualAddressSpace::KernelVAS;}
            inline const VirtualMemoryManager& getVMM() const {return vmm;}
            inline const Util::vector<Util::UniquePtr<Mapping>>& getMappings() const {return mappings;}
            
        private:
            VirtualMemoryManager vmm;
            Util::vector<Util::UniquePtr<Mapping>> mappings;
            static VirtualAddressSpace KernelVAS;
            Mapping findMappingPlaceWithSizeInVAS(uint64_t size);
    };
}