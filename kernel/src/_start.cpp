#include "PreBoot/Bootinfo.h"
#include "KernelUtil.h"
#define STACK_SIZE 4

uint8_t Stack[STACK_SIZE * 0x1000]
__attribute__((aligned(0x1000)));

extern "C" void init_stack(BootInfo* BootInfo, uint64_t stackBase);

void IdentityMapPhysicalMemory(BootInfo* bi){
    //Identity Map Physical RAM
    for(uint64_t addr = 0; addr < memorySizeBytes; addr += 0x1000){
        KernelVMM.MapMemory(addr, addr);
    }

    for(uint64_t addr = reinterpret_cast<uint64_t>(bi->framebuffer->BaseAddress);
        addr < bi->framebuffer->BufferSize; addr++){
        KernelVMM.MapMemory(addr,addr);
    }
}

extern "C" void _start(BootInfo* bootinfo){

    GlobalPageFrameAllocator = PageFrameAllocator(bootinfo->mMap,bootinfo->mMapSize,bootinfo->mMapDescriptorSize);
    uint64_t kernelSizePages = (reinterpret_cast<uint64_t>(&_KernelEnd) - reinterpret_cast<uint64_t>(&_KernelStart)) / 0x1000 + 1;
    GlobalPageFrameAllocator.LockPages(reinterpret_cast<uint64_t>(&_KernelStart),kernelSizePages);
    

    //Sets new stack up and calls main with bootinfo as first parameter
    init_stack(bootinfo,reinterpret_cast<uint64_t>(Stack) + STACK_SIZE * 0x1000); 
}