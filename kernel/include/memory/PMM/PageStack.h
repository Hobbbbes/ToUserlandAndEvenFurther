#pragma once
#include <stdint.h>
#include "../../Util/Bitmap.h"
//Stack grows uppwards in this case
namespace Memory{
class PageStack{
    public:
        PageStack(uint64_t* bufferStart, uint64_t* bufferEnd)
        : bufferStart(bufferStart), bufferEnd(bufferEnd), stackPointer(bufferStart){}

        PageStack() : bufferStart(nullptr) , bufferEnd(nullptr), stackPointer(nullptr){}

        inline void Push(uint64_t addr){
            *stackPointer = addr;
            stackPointer++;
        }

        inline uint64_t Pop(){
            stackPointer--;
            return *(stackPointer);
        }

        inline uint64_t Size() const{
            return reinterpret_cast<uint64_t>(bufferEnd) - reinterpret_cast<uint64_t>(bufferStart);
        }

        inline void InitPush(const Util::Bitmap &pageBitmap){
            for(uint64_t i = 0; i<pageBitmap.Size(); i++){
                if(! pageBitmap[i]){
                    Push(i * 0x1000);
                }
            }
        }
    private:
        uint64_t* bufferStart;
        uint64_t* bufferEnd;
        uint64_t* stackPointer;
}__attribute((aligned(8)));
}