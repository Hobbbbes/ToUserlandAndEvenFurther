#pragma once

namespace Util{
    template<typename T>
    class UniquePtr{
        public:
            UniquePtr(){ptr = nullptr;};
            UniquePtr(T* ptr) : ptr(ptr){}
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
        private:
            T* ptr;
    };
}