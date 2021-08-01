#pragma once
#include <stdint.h>
#include "memory/VMM/PageTable.h"
/*
Memory Map:
0x000000000000 - 0x0fffffffffff Identity Map Physical Memory, only Super User Access 
0x100000000000 - 0x1fffffffffff User Process Code and Data
0x200000000000 - 0x3fffffffffff User Process other mappings (e.g. Stacks, Heaps, Devices etc.)
0x400000000000 - 0x4fffffffffff Kernel Code and Data
0x400000000000 - 0x7fffffffffff Other Kernel Mappings


*/
#define INIT_HEAP_SIZE 10
#define KERNEL_IMAGE_START 0x400000000000
#define HEAP_START 0x500000000000 //64TB

#define STACK_START 0x600000000000 // 96TB
#define STACK_SIZE 20

#define GDT_VIRTUAL_MEMORY_POS 0x700000000000
#define IDT_VIRTUAL_MEMORY_POS 0x700000001000

#define PROCESS_IMAGE_START 0x100000000000
constexpr uint64_t PLM4KernelVASStartIndex = (KERNEL_IMAGE_START >> 39) & 0x1ff;