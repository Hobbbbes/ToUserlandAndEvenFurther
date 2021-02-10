#include "PreBoot/Bootinfo.h"
#define STACK_SIZE 4


uint8_t Stack[STACK_SIZE * 0x1000]
__attribute__((aligned(0x1000)));

extern "C" void init_stack(BootInfo* BootInfo, uint64_t stackBase);
extern "C" void _start(BootInfo* bootinfo){

    //Sets new stack up and calls main with bootinfo as first parameter
    init_stack(bootinfo,reinterpret_cast<uint64_t>(Stack) + STACK_SIZE * 0x1000); 
}