#include "memory.h"
uint64_t memorySizeBytes = 0;
uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize){
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
bool memcmp(const void* first,const  void* second, const uint64_t count){
    for(uint64_t i = 0; i<count; i++){
        if(reinterpret_cast<const uint8_t*>(first)[i] != reinterpret_cast<const uint8_t*>(second)[i]){
            return false;
        }
    }
    return true;
}
bool memcmp(const void* start, const uint8_t val, const uint64_t count){
    for(uint64_t i = 0; i<count;i++){
        if(reinterpret_cast<const uint8_t*>(start)[i] != val){
            return false;
        }
    }
    return true;
}