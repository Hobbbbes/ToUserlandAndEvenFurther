#include "Bitmap.h"
namespace Util{
    Bitmap::Bitmap(const uint64_t bitmapSize,uint8_t* bufferAddr)
    : _Size(bitmapSize), Buffer(bufferAddr)
    {
        for(uint64_t i = 0; i<_Size;i++){
            *reinterpret_cast<uint8_t*>(Buffer + i) = 0;
        }
    }
    Bitmap::Bitmap(): _Size(0), Buffer(nullptr){}
    bool Bitmap::operator[](uint64_t index)const{
        if(index > _Size * 8) return false;
        uint64_t byteIndex = index / 8;
        uint8_t bitIndex = index % 8;
        uint8_t bitIndexer = 0b10000000 >> bitIndex;
        if((Buffer[byteIndex] & bitIndexer) > 0)
            return true;
        return false;

    }

    bool Bitmap::Set(uint64_t index, bool value){
        if(index > _Size * 8) return false;
        uint64_t byteIndex = index / 8;
        uint8_t bitIndex = index % 8;
        uint8_t bitIndexer = 0b10000000 >> bitIndex;
        Buffer[byteIndex] &= ~bitIndexer;
        if(value){
            Buffer[byteIndex] |= bitIndexer;
        }
        return true;
    }
}