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
    Graphics::TextDrawer d(*bootinfo->framebuffer,*bootinfo->psf1_font);
    const char * s = "HELLO WORLD";
    d.print(s);
    while(true);
}