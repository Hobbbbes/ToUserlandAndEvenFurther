#pragma once
#include <stdint.h>
#include "memory/VMM/PageTable.h"
#define INIT_HEAP_SIZE 10
#define KERNEL_IMAGE_START 0x400000000000
#define HEAP_START 0x500000000000 //64TB

#define STACK_START 0x600000000000 // 96TB
#define STACK_SIZE 20

#define GDT_VIRTUAL_MEMORY_POS 0x700000000000
#define IDT_VIRTUAL_MEMORY_POS 0x700000001000

constexpr uint64_t PLM4KernelVASStartIndex = (KERNEL_IMAGE_START >> 39) & 0x1ff;