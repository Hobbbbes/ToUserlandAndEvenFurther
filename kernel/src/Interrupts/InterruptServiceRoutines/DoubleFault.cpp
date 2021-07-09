#include "Interrupts/InterruptServiceRoutines.h"

__attribute__((interrupt)) void Interrupt::Handler::DoubleFault(interrupt_frame* i_frame,uint64_t errorCode){
    Util::Panic("Double Fault");
}