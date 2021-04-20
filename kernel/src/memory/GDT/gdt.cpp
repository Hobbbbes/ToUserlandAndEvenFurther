#include "memory/GDT/gdt.h"
__attribute((aligned(0x1000)))
GDT DefaultGDT = {
    {0,0,0,0x00,0x00,0},//Null
    {0,0,0,0x9a,0xa0,0}, //Kernel Code Segment
    {0,0,0,0x92,0xa0,0}, //Kernel Data Segment
    {0,0,0,0x00,0x00,0}, //User NULL Segment
    {0,0,0,0x9a,0xa0,0}, //User Code Segment
    {0,0,0,0x92,0xa0,0}, //User Data Segment
    //TODO Change privilige Levels for User Segments
};