#pragma once
#include <stdint.h>
//Stack grows uppwards in this case
class PageStack{
    public:
        PageStack(uint64_t* bufferStart, uint64_t* bufferEnd);
        inline uint64_t Pop();
        inline void Push(uint64_t addr);
        inline uint64_t Size() const;
    private:
        uint64_t* bufferStart;
        uint64_t* bufferEnd;
        uint64_t* stackPointer;
};