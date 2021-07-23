#pragma once
#include <stdint.h>
#include "memory/memory.h"
#include "Util/panic.h"
#include <type_traits>
#include <concepts>
namespace Util{
template<typename T> 
class vector{
    private:
        T* buff = nullptr;
        uint64_t size = 0;
        uint64_t capacity = 0;
        void swapBuffers(){
            T* newBuff = reinterpret_cast<T*>(new uint8_t[capacity * sizeof(T)]);
            memcpy(buff,newBuff,size);
            delete[] buff;
            buff = newBuff;
        }
    public:
    
        inline vector(){}
        inline vector(const T& init){buff = new T[capacity](init);}
        inline vector(uint64_t size) : size(size), capacity(size){
            buff = new T[size];
        }
        vector(uint64_t size, T init): size(size), capacity(size){
            buff = new T[size];
            for(uint64_t i = 0; i<size; i++)
                buff[i] = init;
        }
        void push_back(T value){
            if(size + 1 > capacity){
                ++capacity *= 2;
                swapBuffers();
            }
            buff[size++] = value;
        }
        inline T& operator [] (uint64_t index) const {
            #ifdef DEBUG
            if (index >= size)
                Util::Panic("Vector access out of bounds");
            #endif
            return buff[index];
        }
        inline T* begin() const {return buff;}
        inline T* end() const {return buff + size - 1;}
        inline uint64_t getSize() const {return size;}
        inline uint64_t getCapacity() const {return capacity;}
        inline bool contains(const T& v) requires std::equality_comparable<T> {
            for(const T& inVec : this){
                if(inVec == v)
                    return true;
            }
            return false;
        }
        inline void reserve(uint64_t capacity) {
            if(this->capacity >= capacity) return;
            this->capacity = capacity;
            swapBuffers();
        }
};
}