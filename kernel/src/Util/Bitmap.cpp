#include "Bitmap.h"

Bitmap::Bitmap(const uint64_t bitmapSize,uint8_t* bufferAddr)
: _Size(bitmapSize), Buffer(bufferAddr)
{}

bool Bitmap::operator[](uint64_t index){
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

uint64_t Bitmap::Size() const {
    return _Size;
}