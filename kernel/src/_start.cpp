#include "PreBoot/Bootinfo.h"
#include "KernelUtil.h"
#define STACK_SIZE 4

uint8_t Stack[STACK_SIZE * 0x1000]
__attribute__((aligned(0x1000)));

extern "C" void init_stack(BootInfo* BootInfo, uint64_t stackBase);
extern "C" void _start(BootInfo* bootinfo){

    GlobalPageFrameAllocator = PageFrameAllocator(bootinfo->mMap,bootinfo->mMapSize,bootinfo->mMapDescriptorSize);
    uint64_t kernelSizePages = (reinterpret_cast<uint64_t>(&_KernelEnd) - reinterpret_cast<uint64_t>(&_KernelStart)) / 0x1000 + 1;
    GlobalPageFrameAllocator.LockPages(reinterpret_cast<uint64_t>(&_KernelStart),kernelSizePages);
    uint64_t addr = GlobalPageFrameAllocator.RequestPage();

    //Sets new stack up and calls main with bootinfo as first parameter
    init_stack(bootinfo,reinterpret_cast<uint64_t>(Stack) + STACK_SIZE * 0x1000); 
}