#include "PreBoot/Bootinfo.h"
#include "KernelUtil.h"
struct Test{
    uint64_t test1;
    char test2;
    Test* next;
};
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
    Test t = {32,'a', &t};
    Graphics::TextDrawer d(*bootinfo->framebuffer,*bootinfo->psf1_font);
    for(int i = 0; i<128;i++){
        for(int b = 0; b<50000000;b++){}
        d.print<Test,Graphics::TextDrawer::Print_Specifier::Hex>(t).print("\n");
    }
    while(true);
}