#pragma once
#include "PreBoot/EfiMemory.h"
#include "memory/Heap/Heap.h"
#include "memory/PMM/PageFrameAllocator.h"
#include "memory/VMM/VirtualMemoryManager.h"
#include "memory/VMM/VirtualAddressSpace.h"
#include "memory/memory_defines.h"
#include "Util/uniqeptr.h"
uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize);
void memset(void* start, uint8_t value, uint64_t num);
void memset(void* start, uint32_t value, uint64_t num);
bool memcmp(const void* first,const  void* second, const uint64_t count);
bool memcmp(const void* start, const uint8_t val, const uint64_t count);
void memcpy(const void* source,void* dest, const uint64_t count);
extern uint64_t memorySizeBytes;
namespace Util{
    template<typename T>
    inline constexpr std::remove_reference_t<T>&& move(T&& t) noexcept
    {
        return static_cast<std::remove_reference_t<T>&&>(t);
    }
}