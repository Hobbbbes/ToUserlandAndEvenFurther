#pragma once
#include <stddef.h>
#include <stdint.h>
namespace Util{
    class Bitmap{
        public:
            Bitmap(const uint64_t bitmapSize,uint8_t* bufferAddr);
            Bitmap();
            inline uint64_t Size() const {
                return _Size;
            }
            bool operator[](uint64_t index)const;
            bool Set(uint64_t index, bool value);
        private:
            uint8_t* Buffer;
            uint64_t _Size;
    };
}