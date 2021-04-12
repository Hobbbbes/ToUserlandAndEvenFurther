#include "PreBoot/Bootinfo.h"
#include "KernelUtil.h"

void main(BootInfo* bootinfo){
    int* test = new int;
    int* test3 = new int[20];
    int* test2 = test3;
    *test = 0xf0f0f0f0;
    for(int i = 0; i<20;i++){
        *test2 = 0xefefefef;
        test2++;
    }
    delete test;
    delete[] test3;
    Graphics::TextDrawer t(*bootinfo->framebuffer,*bootinfo->psf1_font);
    Util::string s("Hello World");
    s = s + "Hello as well";
    Util::string s2(Util::string("Hello"));
    t.print(std::is_move_constructible<Util::string>::value);
    while(true);
}