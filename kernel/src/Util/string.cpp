#include "Util/string.h"
#include "memory/memory.h"
namespace Util{
    string::~string(){
        delete[] buf;
    }
    string::string(const char* cstr){
        uint64_t s = 1; //1 for null terminator
        char * c = const_cast<char*>(cstr);
        for(char ch = *c;ch != '\0'; c++,ch=*c){
            s++;
        }
        _length = s;
        _cap = s;
        buf = new char[s];
        memcpy(cstr,buf,s);
    }
    string::string() : _length(0) , buf(nullptr){};
    string::string(const string &s){
        buf = new char[s._length];
        _length = s._length;
        _cap = _length;
        memcpy(s.buf,buf,_length);
    }
    string& string::operator =(const string &s){
        if(this == &s){
            return *this;
        }
        if(_cap < s._length){
            delete[] buf;
            buf = new char[s._length];
            _cap = s._length;
        }
        memcpy(s.buf,buf,s._length);
        _length = s._length;
        return *this;
    }
    uint64_t string::length() const{
        return _length; 
    }
    uint64_t string::capacity() const{
        return _cap;
    }
    const char* string::cstr() const{
        return buf;
    }
    string& string::operator +(const string &s){
        if(_length + s._length > _cap){
            char * newBuff = new char[_cap*2 + s._length];
            memcpy(buf,newBuff,_length-1); // Dont copy null terminator
            memcpy(s.buf,newBuff+_length-1, s._length); //Copy in, include null terminator of s -> now null terminator of whole string
            _cap = _cap*2 + s._length;
            buf = newBuff;
        } else {
            memcpy(s.buf,buf+_length-1, s._length);
        }
        _length += s._length-1;
        return *this;
    }
    string& string::operator + (const char* cstr){
        uint64_t s = 1; //1 for null terminator
        char * c = const_cast<char*>(cstr);
        for(char ch = *c;ch != '\0'; c++,ch=*c){
            s++;
        }
        if(_length + s > _cap){
            char * newBuff = new char[_cap*2 + s];
            memcpy(buf,newBuff,_length-1); // Dont copy null terminator
            memcpy(cstr,newBuff+_length-1, s); //Copy in, include null terminator of s -> now null terminator of whole string
            _cap = _cap*2 + s;
            buf = newBuff;
        } else {
            memcpy(cstr,buf+_length-1, s);
        }
        _length += s-1;
        return *this;
    }
    bool string::operator ==(const string &s)const{
        uint64_t minLength = s._length > _length ? _length : s._length;
        for(uint64_t i = 0; i<minLength;i++){
            if(buf[i] != s.buf[i]){
                return false;
            }
        }
        return true;
    }

};