#pragma once
#include <stdint.h>

enum PT_Flag{
    Present = 0,
    ReadWrite = 1,
    UserSupper = 2,
    WriteTrough = 3,
    CachedDiesabled  = 4,
    Accessed = 5,
    Dirty = 6,
    LargePages = 7,
//Bit to Ignore
    Custom0 = 9,
    Custom1 = 10,
    Custom2 = 11,
//Address
    NoExecute = 63 // Only if supported
};

class PageDirectoryEntry{
    private:
        uint64_t value;
    public:
        inline void SetFlag(PT_Flag flag, bool enabled){
            uint64_t bitSelector = static_cast<uint64_t>(1 << flag);
            value &= ~bitSelector;
            if(enabled){
                value |= bitSelector;
            }
        }
        inline bool GetFlag(PT_Flag flag){
            uint64_t bitSelector = static_cast<uint64_t>(1 << flag);
            return value & bitSelector > 0 ? true : false;
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