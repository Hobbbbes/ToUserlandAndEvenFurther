#pragma once
#include <stdint.h>
#include "../PreBoot/EfiMemory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize);
void memset(void* start, uint8_t value, uint64_t num);
void memset(void* start, uint32_t value, uint64_t num);
bool memcmp(const void* first,const  void* second, const uint64_t count);
bool memcmp(const void* start, const uint8_t val, const uint64_t count);
void memcpy(const void* source, const void* dest, const uint64_t count);
extern uint64_t memorySizeBytes;