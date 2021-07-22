#include "Interrupts/IDT/idt.h"
#include "memory/memory.h"
#include "Graphics/print.h"
#include "Interrupts/InterruptServiceRoutines.h"
void Interrupt::IDT_Gate_Descriptor::SetOffset(uint64_t offset){
    offset0 = static_cast<uint16_t>(offset & 0x000000000000ffff);
    offset1 = static_cast<uint16_t>((offset & 0x00000000ffff0000) >> 16);
    offset2 = static_cast<uint32_t>((offset & 0xffffffff00000000) >> 32);
}
void Interrupt::IDT_Gate_Descriptor::SetISTEntry(uint8_t ist){
    IST = ist;
}

void InstallInterruptServiceRoutines(const Interrupt::IDT_Descriptor& idt){
    Interrupt::IDT_Gate_Descriptor* pageFault = reinterpret_cast<Interrupt::IDT_Gate_Descriptor*>(idt.address + 0xE * (sizeof(Interrupt::IDT_Gate_Descriptor)));
    *pageFault = Interrupt::IDT_Gate_Descriptor();
    pageFault->SetOffset(reinterpret_cast<uint64_t>(Interrupt::Handler::PageFault));
    pageFault->Type = IDT_GATE_TYPE_INTERRUPT;

    Interrupt::IDT_Gate_Descriptor* doubleFault = reinterpret_cast<Interrupt::IDT_Gate_Descriptor*>(idt.address + 0x8 * (sizeof(Interrupt::IDT_Gate_Descriptor)));
    *doubleFault = Interrupt::IDT_Gate_Descriptor();
    doubleFault->SetOffset(reinterpret_cast<uint64_t>(Interrupt::Handler::DoubleFault));
    doubleFault->Type = IDT_GATE_TYPE_INTERRUPT;

    Interrupt::IDT_Gate_Descriptor* generalProtectionFault = reinterpret_cast<Interrupt::IDT_Gate_Descriptor*>(idt.address + 0xD * (sizeof(Interrupt::IDT_Gate_Descriptor)));
    *generalProtectionFault = Interrupt::IDT_Gate_Descriptor();
    generalProtectionFault->SetOffset(reinterpret_cast<uint64_t>(Interrupt::Handler::GeneralProtectionFault));
    generalProtectionFault->Type = IDT_GATE_TYPE_INTERRUPT;
}

void Interrupt::SetupIDT(){
    uint64_t pf = Memory::KernelPMM.RequestPage();
    Memory::KernelVMM.MapMemory(IDT_VIRTUAL_MEMORY_POS,pf,Memory::PT_Flag::CachedDisabled);
    Interrupt::IDT_Descriptor desc;
    desc.address = IDT_VIRTUAL_MEMORY_POS;
    desc.size = 0x0fff; //16*255 -1
    InstallInterruptServiceRoutines(desc);
    Graphics::KernelDrawer.print("Setup Interrupt Service Routines\n");
    asm("lidt %0" : : "m" (desc));
    Graphics::KernelDrawer.print("Loaded idtr Register with Idt Descriptor Address: ")
    .print((void*)desc.address).print(" Size: ").print<uint16_t,Graphics::TextDrawer::Print_Specifier::Hex>(desc.size).print("\n");
    asm("sti"); 
    Graphics::KernelDrawer.print("Enabled Interrupts");
}
