#include "Interrupts/InterruptServiceRoutines.h"

__attribute__((interrupt)) void Interrupt::Handler::PageFault(interrupt_frame* i_frame, uint64_t errorCode){
    Util::Panic("Page Fault\n");

    asm("hlt");
}