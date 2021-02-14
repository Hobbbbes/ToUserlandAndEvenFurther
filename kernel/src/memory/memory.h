#pragma once
#include <stdint.h>
#include "../PreBoot/EfiMemory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize);
void memset(void* start, uint8_t value, uint64_t num);
void memset(void* start, uint32_t value, uint64_t num);
extern uint64_t memorySizeBytes = 0;