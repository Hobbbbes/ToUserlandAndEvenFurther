#pragma once

#ifdef DEBUG
#define ASSERT(cond,msg) if(cond){Util::Panic(msg);}
#define Panic(msg) _Panic(msg,__FILE__,__LINE__,__PRETTY_FUNCTION__);
#else
#define ASSERT(cond,msg)
#define Panic(msg) _Panic(msg,__PRETTY_FUNCTION__);
#endif

namespace Util{
    #ifdef DEBUG
    void _Panic(const char* msg, const char* file, int line, const char* func);
    #else
    void _Panic(const char* message = "",const char* caller = __builtin_FUNCTION());
    #endif

}