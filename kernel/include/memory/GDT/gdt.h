#pragma once

#include <stdint.h>

struct GDTDescriptor{
    uint16_t Size;
    uint64_t Offset;
}__attribute__((packed));

struct GDTEntry{
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    uint8_t Accessed :1;
    union {
        uint8_t Writable :1; //Data-Segment Descriptor
        uint8_t Readable :1; //Code-Segment Descriptor
    };
    union {
        uint8_t Expansion_Direction :1; //Data-Segment Descriptor
        uint8_t Conforming :1; //Code-Segment Descriptor
    };
    uint8_t SegmentType :1; //1 in Code Segments, 0 in DataSegments
    uint8_t One :1 = 1;
    uint8_t DPL :2;
    uint8_t Present :1;
    uint8_t Limit1 :4;
    uint8_t AVL :1;
    uint8_t Null :1 = 0;
    union{
        uint8_t Big :1; //Code-Segment Descriptor
        uint8_t Default :1; //Data-Segment Descriptor
    };
    uint8_t Granularity :1;
    uint8_t Base2;
}__attribute__((packed));

struct GDT{
    GDTEntry Null;
    GDTEntry KernelCode;
    GDTEntry KernelData;
    GDTEntry UserNull;
    GDTEntry UserCode;
    GDTEntry UserData;
}__attribute__((packed)) __attribute((aligned(0x1000)));

extern GDT DefaultGDT;

extern "C" void LoadGDT(GDTDescriptor* gdt);