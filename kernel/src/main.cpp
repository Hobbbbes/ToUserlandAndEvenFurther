#include "PreBoot/Bootinfo.h"
#include "KernelUtil.h"
#include "memory/GDT/gdt.h"

void main(BootInfo* bootinfo){
    Graphics::KernelDrawer.print("Sizeof GDTEntry  ").print(sizeof(GDTEntry));
    //GDTEntry* entry = new GDTEntry;
    //memset(entry,(uint8_t)0b10101010,8);
    SetupGDT();
    Graphics::KernelDrawer.print("Setup GDT at ").print((void*)GDT_VIRTUAL_MEMORY_POS).print("\n");
    asm("hlt");
}