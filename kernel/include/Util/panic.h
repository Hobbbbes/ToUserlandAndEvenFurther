#pragma once

namespace Util{
    void Panic(const char* message = "",const char* caller = __builtin_FUNCTION());
    #ifdef DEBUG
    #define ASSERT(cond,msg) if(cond){Util::Panic(msg);}
    #else
    #define ASSERT(cond,msg)
    #endif

}