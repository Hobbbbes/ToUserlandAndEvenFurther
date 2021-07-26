#pragma once
#include <concepts>
namespace Util{
    template<typename T>
    class UniquePtr{
        public:
            UniquePtr(){ptr = nullptr;};
            UniquePtr(T* ptr) : ptr(ptr){}
            template<typename U>
            UniquePtr(U* uptr) requires std::derived_from<U,T> : ptr(reinterpret_cast<T*>(uptr)){}
            UniquePtr(const UniquePtr& uptr) = delete;
            UniquePtr<T>& operator=(const UniquePtr& ptr) = delete;

            UniquePtr(UniquePtr&& uptr){
                this->ptr = uptr->ptr;
                uptr->ptr = nullptr;
            }
            void operator=(UniquePtr&& uptr){
                this->ptr = uptr->ptr;
                uptr->ptr = nullptr;
            }
            inline const T* operator->()const{return ptr;}
            inline T* operator->(){return ptr;}
            inline T& operator*(){return *ptr;};
            inline const T& operator*()const{return *ptr;};
            ~UniquePtr(){
                if(ptr != nullptr)
                    delete ptr;
            }
            T* getPtr(){return ptr;}
            const T* getPtr()const{return ptr;}
            inline bool operator==(const T& v) requires std::equality_comparable<T> {
                return v == *ptr;
            }
            inline bool operator==(const UniquePtr& v) requires std::equality_comparable<T> {
                return *v == *ptr;
            }
        private:
            T* ptr;
    };
}