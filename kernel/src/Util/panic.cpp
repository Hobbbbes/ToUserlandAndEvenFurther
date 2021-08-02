#include "Util/panic.h"
#include "Graphics/print.h"
namespace Util{
    #ifdef DEBUG
    void _Panic(const char* msg, const char* file, int line, const char* func){
        Graphics::KernelDrawer.setColor(Graphics::Red).print("\n\nPANIC in ").
        print(file).print(" at line ").print(line).print("\n In function: ").
        print(func).print("\n\n").print(msg).print("\n\n").print("CPU is halted");

        asm("hlt");
    }
    #else
    void _Panic(const char* message,const char* caller){
        Graphics::KernelDrawer.setColor(Graphics::Red).print("\n\nPANIC in \n").
        print(caller).print("\n\n").print(message).print("\n\n").print("CPU is halted");

        asm("hlt");
    }
    #endif
}   