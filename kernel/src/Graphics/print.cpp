#include "print.h"
#include "../memory/memory.h"
namespace Graphics{
    TextDrawer::TextDrawer(FrameBuffer &fb, PSF1_FONT &font): fb(fb), pos(0,0), font(font){}
    void TextDrawer::putChar(const char c, uint32_t xOff, uint32_t yOff){
        uint32_t* pixPtr = (uint32_t*)fb.BaseAddress;
        char* fontPtr = (char*)font.glyphBuffer + (c * font.psf1_Header->charsize);
        for (uint64_t y = yOff; y<yOff + 16; y++){
            for(uint64_t x = xOff; x<xOff + 8;x++){
                if((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                    *(uint32_t*)(pixPtr + x + (y * fb.PixelsPerScanLine)) = color;
                }
            }
            fontPtr++;
        }
    }

    void TextDrawer::putString(const char* s){
        char *chr = const_cast<char*>(s);
        uint32_t x = 0;
        while(*chr != '\0'){
            if (*chr == '\n'){
                pos.y += 16;
                pos.x = 0;
            } else {
                putChar(*chr,pos.x,pos.y);
                pos.x += 8;
                if(pos.x + 8 > fb.Width){
                    pos.x = 0;
                    pos.y += 16;
                }
            }
            if(pos.y + 16 > fb.Height){
                memcpy(reinterpret_cast<void*>(reinterpret_cast<uint64_t>(fb.BaseAddress) + fb.Width*8*16),fb.BaseAddress,fb.BufferSize);
            }
            chr++;
        }
    }
    
}