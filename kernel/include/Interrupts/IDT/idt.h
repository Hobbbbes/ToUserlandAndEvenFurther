#pragma once
#include <stdint.h>
namespace Interrupt{
    struct IDT_Gate_Descriptor{
        uint16_t offset0;
        uint16_t segmentSelector;
        uint8_t IST :2; //Interrupt Stack Table
        uint8_t null :5 = 0;
        uint8_t Type :4;
        uint8_t null1 :1 = 0;
        uint8_t DPL :2;
        uint8_t Present :1;
        uint16_t offset1;
        uint32_t offset2;
        uint32_t reserved;

    }
}__attribute__((packed));