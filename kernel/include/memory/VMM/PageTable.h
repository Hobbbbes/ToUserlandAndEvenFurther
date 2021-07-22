#pragma once
#include <stdint.h>
namespace Memory{
enum PT_Flag{
    Present = 1<<0,
    ReadWrite = 1<<1,
    UserSupper = 1<<2,
    WriteTrough = 1<<3,
    CachedDisabled  = 1<<4,
    Accessed = 1<<5,
//Bit to Ignore
    LargePages = 1<<7,
//Bit to Ignore
    Custom0 = 1<<9,
    Custom1 = 1<<10,
    Custom2 = 1<<11,
//Address
    NoExecute = 1L<<63 // Only if supported
};

class PageDirectoryEntry{
    private:
        uint64_t value;
    public:
        inline void SetFlag(uint64_t flags,bool set_unset){
            value &= ~flags;
            if(set_unset){
                value |= flags;
            }
        }
        inline bool GetFlag(PT_Flag flag){
            return (value & flag) > 0 ? true : false;
        }
        inline void SetAddress(uint64_t addr){
            addr &= 0x000000ffffffffff;
            value &= 0xfff0000000000fff;
            value |= (addr << 12);
        }
        inline uint64_t GetAddress(){
            return (value & 0x000ffffffffff000) >> 12;
        }
        inline void clear(){
            value = 0;
        }
};

struct PageTable{
    PageDirectoryEntry entries[512];
}__attribute__((aligned(0x1000)));
}