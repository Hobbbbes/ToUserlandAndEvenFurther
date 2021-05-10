#include "Util/panic.h"
#include "Graphics/print.h"
namespace Util{
    void Panic(const char* message,const char* caller){
        Graphics::KernelDrawer.setColor(Graphics::Red).print("\n\nPANIC  ").
        print(caller).print("\n").print(message).print("\n").print("CPU is halted");

        asm("hlt");
    }
}   