#include "Interrupts/InterruptServiceRoutines.h"
__attribute__((interrupt)) void Interrupt::Handler::GeneralProtectionFault(interrupt_frame* i_frame, uint64_t errorCode){
    Util::Panic("General Protection Fault");
}