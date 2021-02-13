#include "memory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize){
    static uint64_t memorySizeBytes = 0;
    if(memorySizeBytes > 0) return memorySizeBytes;
    for(uint64_t i = 0; i<mMapEntries ; i++){
        EFI_MEMORY_DESCRIPTOR* desc = reinterpret_cast<EFI_MEMORY_DESCRIPTOR*>(reinterpret_cast<uint64_t>(mMap) + (i * mMapDescSize));
        memorySizeBytes += desc->numPages * 4096;
    }
    return memorySizeBytes;
}

void memset(void* start, uint8_t value, uint64_t num){
    for(uint64_t i = 0; i < num ; i++){
        *reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>(start) + i) = value;
    }
}

void memset(void* start, uint32_t value, uint64_t num){
    for(uint64_t i = 0; i < num ; i++){
        *reinterpret_cast<uint32_t*>(reinterpret_cast<uint64_t>(start) + i) = value;
    }
}