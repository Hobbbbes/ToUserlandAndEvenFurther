#pragma once
#include "Util/vector.h"
#include "memory/memory.h"
#include "Util/uniqeptr.h"
#include "PreBoot/Bootinfo.h"
extern "C" [[noreturn]] void _start(BootInfo* bootinfo);
namespace Memory {
    class Mapping {
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
                Special
            };
            enum class MappingType : uint8_t{
                All,
                OnDemand,
                Physical
            };
            Mapping(uint64_t vstart, uint64_t size, Type type, MappingType mappingType, bool kernel = false);
            inline uint64_t getStart() const {return start;}
            inline uint64_t getEnd() const {return start + (size*0x1000) - 1;}
            inline Type getType() const {return type;}
            inline uint64_t getSize() const {return size;}
            inline bool kernelMapping() const {return kernel;}
            inline bool containsAddress(uint64_t addr) const {return addr >= getStart() && addr <= getEnd();}
            inline bool operator == (const Mapping &other) const {return other.start == start && other.size == size && other.type == type;}
            inline bool operator != (const Mapping &other) const {return ! (other == *this);}
            inline void setSize(uint64_t size) {this->size = size;}
            virtual void map(VirtualMemoryManager& vmm)const;
            virtual void unmap(VirtualMemoryManager &vmm)const;
        protected:
            uint64_t start;
            uint64_t size;
            Type type;
            MappingType mappingType;
            bool kernel;
            bool mapped;
        friend VirtualAddressSpace;
    };
    
    class DeviceMapping : public Mapping {
        public:
            DeviceMapping(uint64_t vstart, uint64_t size, uint64_t physicalStart, bool kernel = false);
            inline uint64_t getPhysicalStart() const {return physicalStart;}
            void map(VirtualMemoryManager &vmm) const override;
            void unmap(VirtualMemoryManager &vmm) const override;
        private:
            uint64_t physicalStart;
        
    };
/*
    class FileMapping : public Mapping {
        public:
            FileMapping(uint64_t vstart, uint64_t size, bool kernel = false);
            void map(VirtualMemoryManager &mm) override;
            //map file somehow
        private:
            //File Handel
    };
*/ 
    class PhysicalMapping : public Mapping {
        public:
            PhysicalMapping(uint64_t vstart,const Type type, const Util::vector<uint64_t>& physicalAddresses, bool kernel = false);
            void map(VirtualMemoryManager &vmm) const override;
            void unmap(VirtualMemoryManager &vmm) const override;
        private:
            Util::vector<uint64_t> physicalAddresses;
    };

    class SpecialMapping : public Mapping {
        public:
            SpecialMapping(uint64_t vstart,uint64_t size, uint64_t flags);
            void map(VirtualMemoryManager &vmm) const override;
            void unmap(VirtualMemoryManager &vmm) const override;
        private:
            uint64_t flags;
    };

    class VirtualAddressSpace{
        public:
            inline VirtualAddressSpace(VirtualMemoryManager& vmmManager) : vmm(vmmManager){}
            inline VirtualAddressSpace(VirtualMemoryManager&& vmmManager) : vmm(vmmManager){}
            uint64_t mappingForAddressIndex(uint64_t addr) const;
            inline const Mapping& mappingByIndex(uint64_t index) const {return *mappings[index];}
            inline const Mapping& mappingForAddress(uint64_t addr) const {
                return mappingByIndex(mappingForAddressIndex(addr));
            }
            void map(Util::UniquePtr<Mapping>&& mapping);
            void unmap(const Mapping& mapping);
            static VirtualAddressSpace newUserVAS();
            inline static VirtualAddressSpace& getKernelVAS() {return VirtualAddressSpace::KernelVAS;}
            inline static void initKernelVAS(VirtualMemoryManager& vmm) {KernelVAS = VirtualAddressSpace(vmm);}
            inline const VirtualMemoryManager& getVMM() const {return vmm;}
            inline const Util::vector<Util::UniquePtr<Mapping>>& getMappings() const {return mappings;}
            
        private:
            uint8_t test;
            VirtualMemoryManager vmm;
            Util::vector<Util::UniquePtr<Mapping>> mappings;
            static VirtualAddressSpace KernelVAS;
            Mapping findMappingPlaceWithSizeInVAS(uint64_t size);
        friend void ::_start(BootInfo* bootinfo);
    };
}
