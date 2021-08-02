#include "PreBoot/Bootinfo.h"
#include "KernelUtil.h"
#include "memory/GDT/gdt.h"
#include "Interrupts/IDT/idt.h"

void main(BootInfo* bootinfo){
    Memory::SetupGDT();
    Graphics::KernelDrawer.print("Setup GDT at ").print((void*)GDT_VIRTUAL_MEMORY_POS).print("\n");
    
    Interrupt::SetupIDT();

    //int a = *reinterpret_cast<int*>(0x1000000000000);
    asm("hlt");
}