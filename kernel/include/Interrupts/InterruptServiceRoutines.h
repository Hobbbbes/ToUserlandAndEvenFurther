#pragma once
#include <stdint.h>
#include "Util/panic.h"
namespace Interrupt{
    namespace Handler{
        struct interrupt_frame;
        __attribute__((interrupt)) void PageFault(interrupt_frame* i_frame, uint64_t errorCode);
        __attribute__((interrupt)) void DoubleFault(interrupt_frame* i_frame,uint64_t errorCode);
        __attribute__((interrupt)) void GeneralProtectionFault(interrupt_frame* i_frame, uint64_t errorCode);
    }
}