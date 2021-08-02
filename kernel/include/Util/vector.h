#pragma once
#include <stdint.h>
#include "memory/memory.h"
#include "Util/panic.h"
#include <type_traits>
#include <concepts>
#include <utility>
namespace Util{
template<typename T> 
class vector{
    private:
        T* buff = nullptr;
        uint64_t size = 0;
        uint64_t capacity = 0;
        void swapBuffers(){
            T* newBuff = reinterpret_cast<T*>(new uint8_t[capacity * sizeof(T)]);
            if(buff != nullptr){
                for(uint64_t i = 0; i<size;i++){
                    newBuff[i] = std::move(buff[i]);
                    buff[i].~T();
                }
                delete buff;
            } else {
                for(uint64_t i = 0; i<size;i++)
                    newBuff[i] = std::move(buff[i]);
            }

            buff = newBuff;
        }
        void destroyBuff(){
            for(uint64_t i = 0; i < size; i++){
                buff[i].~T();
            }
            delete buff;
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

        vector(const vector& vec){
            if(buff != nullptr)
                destroyBuff();
            buff = new T[vec.capacity];
            size = vec.size;
            capacity = vec.capacity;
            for(uint64_t i = 0; i<vec.size;i++)
                buff[i] = vec[i];
            
        }

        vector& operator=(const vector& vec){
            if(buff != nullptr)
                destroyBuff();
            buff = new T[vec.capacity];
            size = vec.size;
            capacity = vec.capacity;
            for(uint64_t i = 0; i<vec.size;i++)
                buff[i] = vec[i];
            return *this;
        }
        vector(vector&& vec){
            buff = vec.buff;
            size = vec.size;
            capacity = vec.capacity;
            vec.buff = nullptr;
            vec.size = 0;
            vec.capacity = 0;
        }
        
        vector& operator=(vector&& vec){
            buff = vec.buff;
            size = vec.size;
            capacity = vec.capacity;
            vec.buff = nullptr;
            vec.size = 0;
            vec.capacity = 0;
            return *this;
        }

        void push_back(T&& value) requires std::move_constructible<T> {
            
            if(size + 1 > capacity){
                ++capacity *= 2;
                swapBuffers();
            }
            buff[size++] = std::move(value);
        }

        void push_back(const T& value) requires std::copy_constructible<T> {
            if(size + 1 > capacity){
                ++capacity *= 2;
                swapBuffers();
            }
            buff[size++](value);
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
        bool contains(const T& v) const requires std::equality_comparable<T> {
            for(const T& inVec : *this){
                if(inVec == v)
                    return true;
            }
            return false;
        }
        template<typename U>
        bool contains(const U& v) const requires requires (const T& x, const U& y) {x==y;} {
            for(const T& inVec : *this){
                if(inVec == v)
                    return true;
            }
            return false;
        }
        void reserve(uint64_t capacity) {
            if(this->capacity >= capacity) return;
            this->capacity = capacity;
            swapBuffers();
        }
        void remove(const T& v) requires std::equality_comparable<T> {
            for(uint64_t i = 0; i < size ; i++){
                if(buff[i] == v)
                    remove(i);

            }
        }
        template<typename U>
        void remove(const U& v) { //requires std::equality_comparable_with<T,U>
            for(uint64_t i = 0; i < size ; i++){
                if(buff[i] == v)
                    remove(i);
            }
        }

        void remove(uint64_t index) {
            for(uint64_t i = index + 1; i < size; i++){
                buff[i-1] = move(buff[i]);
            }
            size -= 1;
        }

};
}