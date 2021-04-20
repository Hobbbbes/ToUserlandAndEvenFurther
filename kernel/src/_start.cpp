#include "PreBoot/Bootinfo.h"
#include "KernelUtil.h"
#include "Graphics/print.h"
#define INIT_HEAP_SIZE 10
#define HEAP_START 0x400000000000 //64TB
#define STACK_START 0x600000000000 // 96TB
#define STACK_SIZE 20

extern "C" void init_stack(BootInfo* BootInfo, uint64_t stackBase);

void IdentityMapPhysicalMemory(BootInfo* bi){
    //Identity Map Physical RAM
    for(uint64_t addr = 0; addr < memorySizeBytes; addr += 0x1000){
        KernelVMM.MapMemory(addr, addr);
    }

    for(uint64_t addr = reinterpret_cast<uint64_t>(bi->framebuffer->BaseAddress);
        addr < bi->framebuffer->BufferSize + reinterpret_cast<uint64_t>(bi->framebuffer->BaseAddress); addr += 0x1000){
        KernelVMM.MapMemory(addr,addr);
    }
    memset(bi->framebuffer->BaseAddress,(uint8_t)0,bi->framebuffer->BufferSize);
}

void InitHeap(){
    for(uint64_t i = 0; i<INIT_HEAP_SIZE;i++){
        uint64_t page = KernelPMM.RequestPage();
        KernelVMM.MapMemory(HEAP_START + (i * 0x1000), page);
    }
    initHeap(HEAP_START,HEAP_START + (INIT_HEAP_SIZE * 0x1000));
}

void MapStack(){
    for(uint64_t i = 0;i<=STACK_SIZE;i++){
        uint64_t page = KernelPMM.RequestPage();
        KernelVMM.MapMemory(STACK_START + (i * 0x1000), page);
    }
}

extern "C" void _start(BootInfo* bootinfo) {
    Graphics::KernelDrawer = Graphics::TextDrawer(*bootinfo->framebuffer,*bootinfo->psf1_font);
    
    KernelPMM = PageFrameAllocator(bootinfo->mMap,bootinfo->mMapSize,bootinfo->mMapDescriptorSize);
    
    uint64_t kernelSizePages = (reinterpret_cast<uint64_t>(&_KernelEnd) - reinterpret_cast<uint64_t>(&_KernelStart)) / 0x1000 + 1;
    
    KernelPMM.LockPages(reinterpret_cast<uint64_t>(&_KernelStart),kernelSizePages); 
    
    uint64_t PLM4 = KernelPMM.RequestPage();
    memset(reinterpret_cast<void*>(PLM4), (uint8_t)0,0x1000);
    KernelVMM = VirtualMemoryManager(reinterpret_cast<PageTable*>(PLM4));

    IdentityMapPhysicalMemory(bootinfo);
    asm("mov %0,%%cr3" : : "r"(PLM4));

    Graphics::KernelDrawer.print("Init PMM\n");
    Graphics::KernelDrawer.print("Kernel binary takes ").print(kernelSizePages).print(" Pages\n");
    Graphics::KernelDrawer.print("Locked Kernel Pages\n");
    Graphics::KernelDrawer.print("Setup Virtual Memory Manager for the Kernel with PLM4 at ").
    print((void*)PLM4).print("\n");

    Graphics::KernelDrawer.print("Init Heap at ").print((void*) HEAP_START).print("\n");
    InitHeap();
    Graphics::KernelDrawer.print("Map Stack at ").print((void*) STACK_START).print("\n");
    MapStack();
    Graphics::KernelDrawer.print("Change to new Stack\n");
    //Sets new stack up and calls main with bootinfo as first parameter
    init_stack(bootinfo,(STACK_START) + (STACK_SIZE * 0x1000)); 
}