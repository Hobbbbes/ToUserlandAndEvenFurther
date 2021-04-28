#pragma once
#include <stdint.h>
namespace Interrupt{
    struct IDT_Gate_Descriptor{
        uint16_t offset0;
        uint16_t segmentSelector = 0x8; // Kernel Code Selector
        uint8_t IST :3; //Interrupt Stack Table
        uint8_t null :5 = 0;
        uint8_t Type :4;
        uint8_t null1 :1 = 0;
        uint8_t DPL :2 = 0;
        uint8_t Present :1 = 1;
        uint16_t offset1;
        uint32_t offset2;
        uint32_t reserved;
        void SetOffset(uint64_t offset);
        void SetISTEntry(uint8_t ist);
    }__attribute__((packed));

    struct IDT_Descriptor{
        uint16_t size;
        uint64_t address;
    }__attribute__((packed));

    IDT_Descriptor SetupIDT();
    void InstallInterruptServiceRoutines(const IDT_Descriptor& idt);


}

    #define IDT_GATE_TYPE_INTERRUPT (0b1110)
    #define IDT_GATE_TYPE_CALL (0b1100)
    #define IDT_GATE_TYPE_TRAP (0b1111)