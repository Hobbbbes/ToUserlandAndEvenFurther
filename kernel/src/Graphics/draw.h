#pragma once
#include "../PreBoot/Framebuffer.h"
#include "../PreBoot/PSF1_Font.h"
#include "../Util/Vector2.h"
class TextDrawer{
    public:
        TextDrawer(FrameBuffer &fb, PSF1_FONT &font);
        void putString(const char* s);
        void putChar(const char c);
    private:
        FrameBuffer fb;
        Util::Vector2<uint32_t> pos;
        PSF1_FONT font;
};