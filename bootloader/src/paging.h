#pragma once

#include <efi.h>

void MapEfiMemoryMap(UINTN MapSize, UINTN DescriptorSize, EFI_MEMORY_DESCRIPTOR* Map, void* PLM4);