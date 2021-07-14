#pragma once

#include <efi.h>

void MapEfiMemoryMap(UINTN MapSize, UINTN DescriptorSize, EFI_MEMORY_DESCRIPTOR* Map, void* PLM4);
void MapMemory(void* PLM4, uint64_t paddr, uint64_t vaddr);