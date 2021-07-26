#include "memory/GDT/gdt.h"
#include "memory/memory.h"
namespace Memory{
extern "C" void LoadGDT(GDTDescriptor* gdt);
void SetupGDT(){
    //uint64_t pf = KernelPMM.RequestPage();
    //KernelVMM.MapMemory(GDT_VIRTUAL_MEMORY_POS,pf);
    Memory::VirtualAddressSpace::getKernelVAS().map(
        Util::UniquePtr<Memory::Mapping>(
            new SpecialMapping(GDT_VIRTUAL_MEMORY_POS,1,Memory::PT_Flag::ReadWrite | Memory::PT_Flag::CachedDisabled | Memory::PT_Flag::UserSupper)
        )
    );
    GDT* gdt = reinterpret_cast<GDT*>(GDT_VIRTUAL_MEMORY_POS);
    memset(gdt,(uint32_t)0,0x1000/32); //Set page and therefor Null Segments to 0
    gdt->KernelCode = GDTEntry();
    gdt->KernelData = GDTEntry();
    gdt->UserCode = GDTEntry();
    gdt->UserData = GDTEntry();

    gdt->KernelCode.flags.DPL = 0;
    gdt->KernelCode.flags.Present = 1;
    gdt->KernelCode.flags.Read_Write = 1;
    gdt->KernelCode.flags.SegmentType = 1;

    gdt->KernelData.flags.DPL = 0;
    gdt->KernelData.flags.Present = 1;
    gdt->KernelData.flags.Read_Write = 1;

    gdt->UserCode.flags.DPL = 3;
    gdt->UserCode.flags.Present = 1;
    gdt->UserCode.flags.Read_Write = 1;
    gdt->UserCode.flags.SegmentType = 1;

    gdt->UserData.flags.DPL = 3;
    gdt->UserData.flags.Present = 1;
    gdt->UserData.flags.Read_Write = 1;

    GDTDescriptor gdtDesc;
    gdtDesc.Offset = reinterpret_cast<uint64_t>(gdt);
    gdtDesc.Size = sizeof(GDT) - 1;
    LoadGDT(&gdtDesc);
}
}