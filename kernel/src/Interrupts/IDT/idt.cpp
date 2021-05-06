#include "Interrupts/IDT/idt.h"
#include "memory/memory.h"
#include "Graphics/print.h"
void Interrupt::IDT_Gate_Descriptor::SetOffset(uint64_t offset){
    offset0 = static_cast<uint16_t>(offset & 0x000000000000ffff);
    offset1 = static_cast<uint16_t>((offset & 0x00000000ffff0000) >> 16);
    offset2 = static_cast<uint32_t>((offset & 0xffffffff00000000) >> 32);
}
void Interrupt::IDT_Gate_Descriptor::SetISTEntry(uint8_t ist){
    IST = ist;
}

void InstallInterruptServiceRoutines(const Interrupt::IDT_Descriptor& idt){

}

void Interrupt::SetupIDT(){
    uint64_t pf = KernelPMM.RequestPage();
    KernelVMM.MapMemory(IDT_VIRTUAL_MEMORY_POS,pf,PT_Flag::CachedDisabled);
    Interrupt::IDT_Descriptor desc;
    desc.address = IDT_VIRTUAL_MEMORY_POS;
    desc.size = 0x0fff; //16*255 -1
    InstallInterruptServiceRoutines(desc);
    Graphics::KernelDrawer.print("Setup Interrupt Service Routines\n");
    asm("lidt %0" : : "m" (desc));
    Graphics::KernelDrawer.print("Loaded idtr Register with Idt Descriptor Address:")
    .print(desc.address).print(" Size: ").print(desc.size).print("\n");
    asm("sti"); 
    Graphics::KernelDrawer.print("Enabled Interrupts");
}
