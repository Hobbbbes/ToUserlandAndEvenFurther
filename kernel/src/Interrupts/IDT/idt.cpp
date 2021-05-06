#include "Interrupts/IDT/idt.h"
#include "memory/memory.h"
void Interrupt::IDT_Gate_Descriptor::SetOffset(uint64_t offset){
    offset0 = static_cast<uint16_t>(offset & 0x000000000000ffff);
    offset1 = static_cast<uint16_t>((offset & 0x00000000ffff0000) >> 16);
    offset2 = static_cast<uint32_t>((offset & 0xffffffff00000000) >> 32);
}
void Interrupt::IDT_Gate_Descriptor::SetISTEntry(uint8_t ist){
    IST = ist;
}

Interrupt::IDT_Descriptor Interrupt::SetupIDT(){
    uint64_t pf = KernelPMM.RequestPage();
}
void Interrupt::InstallInterruptServiceRoutines(const IDT_Descriptor& idt){

}