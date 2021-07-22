#pragma once
#include <stdint.h>
#include "memory/VMM/VirtualMemoryManager.h"
#include "Util/vector.h"
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
                Device
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
            inline bool containsAddress(uint64_t addr) const {return addr >= getStart() && addr <= getEnd();}
        private:
            uint64_t start;
            uint64_t size;
            Type type;
            MappingType mappingType;
            bool kernel;
    };
    class DeviceMemoryMapping : public Mapping {
        public:
            DeviceMemoryMapping(uint64_t start, uint64_t size, uint64_t physicalStart, bool kernel = false);
        private:
            uint64_t physicalStart;
    };

    class FileMemoryMapping : public Mapping {
        public:
            FileMemoryMapping(uint64_t start, uint64_t size, bool kernel = false);
        private:
            //File Handel
    };
    class VirtualAddressSpace{
        public:
            inline VirtualAddressSpace(VirtualMemoryManager vmmManager) : vmmManager(vmmManager){}
            uint64_t mappingForAddressIndex(uint64_t addr) const;
            inline const Mapping& mappingByIndex(uint64_t index) const {return mappings[index];}
            inline const Mapping& mappingForAddress(uint64_t addr) const {
                return mappingByIndex(mappingForAddressIndex(addr));
            }
            void map(const Mapping& mapping);
            void unmap(const Mapping& mapping);
            
            
        private:
            VirtualMemoryManager vmmManager;
            Util::vector<Mapping> mappings;

        friend Mapping;
    };
}