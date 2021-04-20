#include "PreBoot/Bootinfo.h"
#include "KernelUtil.h"
#include "memory/GDT/gdt.h"
void main(BootInfo* bootinfo){
    Graphics::KernelDrawer.print(sizeof(GDTEntry));
    while(true);
}