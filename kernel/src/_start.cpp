#include "PreBoot/Bootinfo.h"
#include "KernelUtil.h"
#include "Graphics/print.h"
#include "memory/memory_defines.h"
#include "memory/VMM/VirtualAddressSpace.h"
extern "C" [[noreturn]] void init_stack(BootInfo* BootInfo, uint64_t stackBase);

void IdentityMapPhysicalMemory(BootInfo* bi){
    //Identity Map Physical RAM*
    /*
    for(uint64_t addr = 0; addr < memorySizeBytes; addr += 0x1000){
        Memory::KernelVMM.MapMemory(addr, addr);
    }

    for(uint64_t addr = reinterpret_cast<uint64_t>(bi->framebuffer->BaseAddress);
        addr < bi->framebuffer->BufferSize + reinterpret_cast<uint64_t>(bi->framebuffer->BaseAddress); addr += 0x1000){
        Memory::KernelVMM.MapMemory(addr,addr);
    }()
    */
   //Map physical memory
   Memory::VirtualAddressSpace::getKernelVAS().map(
       Util::UniquePtr<Memory::Mapping>(
           new Memory::DeviceMapping(0,memorySizeBytes / 0x1000,0,true)
       )
   );
   Memory::VirtualAddressSpace::getKernelVAS().map(
       Util::UniquePtr<Memory::Mapping>(
           new Memory::DeviceMapping(reinterpret_cast<uint64_t>(bi->framebuffer->BaseAddress),
           bi->framebuffer->BufferSize / 0x1000,reinterpret_cast<uint64_t>(bi->framebuffer->BaseAddress),true)
       )
   );
}

void InitHeap(Memory::VirtualMemoryManager& vmm, Memory::VirtualMemoryManager& old_vmm){
    for(uint64_t i = 0; i<INIT_HEAP_SIZE;i++){
        uint64_t page = Memory::PageFrameAllocator::getPMM().RequestPage();
        vmm.MapMemory(HEAP_START + (i * 0x1000), page, Memory::PT_Flag::ReadWrite | Memory::PT_Flag::UserSupper);
        old_vmm.MapMemory(HEAP_START + (i * 0x1000), page, Memory::PT_Flag::ReadWrite | Memory::PT_Flag::UserSupper);
    }
    Memory::initHeap(HEAP_START,HEAP_START + (INIT_HEAP_SIZE * 0x1000));
}

void MapStack(){
    /*
    for(uint64_t i = 0;i<=STACK_SIZE;i++){
        uint64_t page = Memory::PageFrameAllocator::getPMM().RequestPage();
        Memory::KernelVMM.MapMemory(STACK_START + (i * 0x1000), page);
    }
    */
   Memory::VirtualAddressSpace::getKernelVAS().map(
       Util::UniquePtr<Memory::Mapping>(
           new Memory::Mapping(STACK_START,STACK_SIZE,Memory::Mapping::Type::ProcessStack,Memory::Mapping::MappingType::All,true)
       )
   );
}

extern "C" [[noreturn]] void _start(BootInfo* bootinfo) {
    memset(bootinfo->framebuffer->BaseAddress,(uint8_t)0,bootinfo->framebuffer->BufferSize);
    Graphics::KernelDrawer = Graphics::TextDrawer(*bootinfo->framebuffer,*bootinfo->psf1_font);
    Graphics::KernelDrawer.print("In Kernel\n");
    Memory::PageFrameAllocator::KernelPMM = Memory::PageFrameAllocator(bootinfo->mMap,bootinfo->mMapSize,bootinfo->mMapDescriptorSize);

    uint64_t kernelSizePages = (reinterpret_cast<uint64_t>(&_KernelEnd) - reinterpret_cast<uint64_t>(&_KernelStart)) / 0x1000 + 1;
    uint64_t kernelROSizePages = (reinterpret_cast<uint64_t>(&_KernelROEnd) - reinterpret_cast<uint64_t>(&_KernelStart)) / 0x1000 + 1;
    Memory::PageFrameAllocator::getPMM().LockPages(reinterpret_cast<uint64_t>(&_KernelStart) - KERNEL_IMAGE_START,kernelSizePages); 
    
    uint64_t PLM4 = Memory::PageFrameAllocator::getPMM().RequestPage();
    memset(reinterpret_cast<void*>(PLM4),(uint8_t)0,0x1000);
    Memory::VirtualMemoryManager vmm(reinterpret_cast<Memory::PageTable*>(PLM4)); 
    uint64_t old_PLM4;
    asm("mov %%cr3,%0" : "=r" (old_PLM4) : : );
    Memory::VirtualMemoryManager old_vmm(reinterpret_cast<Memory::PageTable*>(old_PLM4));

    InitHeap(vmm, old_vmm);
    Memory::VirtualAddressSpace::initKernelVAS(vmm);
    //Heap Mapping
    Memory::VirtualAddressSpace::getKernelVAS().mappings.push_back(
        Util::UniquePtr(new Memory::Mapping(HEAP_START,INIT_HEAP_SIZE,Memory::Mapping::Type::ProcessData,Memory::Mapping::MappingType::All,true))
    );
    Util::vector<uint64_t> phyiscalAddr;
    phyiscalAddr.reserve(kernelROSizePages);
    for(uint64_t i = 0; i<kernelROSizePages;i++)
        phyiscalAddr.push_back((reinterpret_cast<uint64_t>(&_KernelStart) - KERNEL_IMAGE_START)+ i*0x1000);
    Memory::VirtualAddressSpace::getKernelVAS().map(
        Util::UniquePtr<Memory::Mapping>(new Memory::PhysicalMapping((uint64_t)&_KernelStart,Memory::Mapping::Type::ProcessCode,phyiscalAddr,true))
    );

    Util::vector<uint64_t> phyiscalDataAddr;
    phyiscalDataAddr.reserve(kernelSizePages - kernelROSizePages);
    uint64_t _kernelROStart = (uint64_t)&_KernelROEnd + (0x1000 - ((uint64_t)&_KernelROEnd % 0x1000));
    for(uint64_t i = 0; i<kernelSizePages - kernelROSizePages;i++)
        phyiscalDataAddr.push_back((_kernelROStart - KERNEL_IMAGE_START)+ i*0x1000);
    Memory::VirtualAddressSpace::getKernelVAS().map(
        Util::UniquePtr<Memory::Mapping>(new Memory::PhysicalMapping(_kernelROStart,Memory::Mapping::Type::ProcessData,phyiscalDataAddr,true))
    );

    IdentityMapPhysicalMemory(bootinfo);
    MapStack();
    asm("mov %0,%%cr3" : : "r"(PLM4));

    Graphics::KernelDrawer.print("Init PMM\n");
    Graphics::KernelDrawer.print("Kernel binary takes ").print(kernelSizePages).print(" Pages\n");
    Graphics::KernelDrawer.print("Locked Kernel Pages\n");
    Graphics::KernelDrawer.print("Setup Virtual Memory Manager for the Kernel with PLM4 at ").
    print((void*)PLM4).print("\n");

    Graphics::KernelDrawer.print("Init Heap at ").print((void*) HEAP_START).print("\n");
    Graphics::KernelDrawer.print("Map Stack at ").print((void*) STACK_START).print("\n");
    Graphics::KernelDrawer.print("Change to new Stack\n");
    //Sets new stack up and calls main with bootinfo as first parameter
    init_stack(bootinfo,(STACK_START) + (STACK_SIZE * 0x1000)); 
}