#include "PageStack.h"

PageStack::PageStack(uint64_t* bufferStart, uint64_t* bufferEnd)
: bufferStart(bufferStart), bufferEnd(bufferEnd), stackPointer(bufferStart)
{}

void PageStack::Push(uint64_t addr){
    *stackPointer = addr;
    stackPointer++;
}

uint64_t PageStack::Pop(){
    stackPointer--;
    return *(stackPointer + 1);
}

uint64_t PageStack::Size() const{
    return reinterpret_cast<uint64_t>(bufferEnd) - reinterpret_cast<uint64_t>(bufferStart);
}