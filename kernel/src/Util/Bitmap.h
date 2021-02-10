#pragma once
#include <stddef.h>
#include <stdint.h>

class Bitmap{
    public:
        Bitmap(const uint64_t bitmapSize,uint8_t* bufferAddr);
        uint64_t Size() const;
        bool operator[](uint64_t index);
        inline bool Set(uint64_t index, bool value);
    private:
        uint8_t* Buffer;
        uint64_t _Size;
};