#pragma once
#include <stdint.h>
namespace Util{
    class string{
        private:
            char * buf;
            uint64_t _length;
            uint64_t _cap;
        public:
            ~string();
            string(const char* cstr);
            string();
            string(const string &s);
            string& operator =(const string &s);
            //string(const string &&s);
            //void operator =(const string &&s);
            uint64_t length() const;
            uint64_t capacity() const;
            const char* cstr()const;
            string& operator + (const string &s);
            string& operator + (const char* s);
            bool operator ==(const string &s)const;
    };
};