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
    struct Flags {
        uint8_t Accessed :1;
        uint8_t Read_Write :1; //Write in Data-Segment Descriptor, Read in Code-Segment Descriptor
        uint8_t ExpansionDirection_Conforming :1; // Expansion_Direction in Data-Segment Descriptor, Conforming in Code-Segment Descriptor
        uint8_t SegmentType :1; //1 in Code Segments, 0 in DataSegments, executable
        uint8_t DescriptorType :1 = 1; //1 for both Code and Data Segment
        uint8_t DPL :2;
        uint8_t Present :1;
        uint8_t Limit1 :4;
        uint8_t AVL :1;
        uint8_t LongMode :1 = 1; //Is 64 Bit Segment
        uint8_t Big_Default :1; //Big in Code-Segment Descriptor, Default in Data-Segment Descriptor
        uint8_t Granularity :1;
    };
    Flags flags;
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

void SetupGDT();
