#pragma once
#include "../PreBoot/Framebuffer.h"
#include "../PreBoot/PSF1_Font.h"
#include "../Util/Vector2.h"
#include <type_traits>
#include <limits>
namespace Graphics{
    class TextDrawer{
        public:
            TextDrawer(FrameBuffer &fb, PSF1_FONT &font);
            TextDrawer& setColor(const uint32_t c);
            enum class Print_Specifier{
                Hex,Standart
            };
            template<typename T,Print_Specifier S = Print_Specifier::Standart>
            TextDrawer& print(const T v){
                if constexpr((std::is_same<T, const char*>::value || std::is_same<T,char*>::value) && S == Print_Specifier::Standart){
                    putString(v);
                } else if constexpr(std::is_same<T, uint64_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, uint16_t>::value || std::is_same<T, uint8_t>::value
                || std::is_same<T, int64_t>::value || std::is_same<T, int32_t>::value || std::is_same<T, int16_t>::value|| std::is_same<T, int8_t>::value){
                    if constexpr(S == Print_Specifier::Hex){
                        printIntHex<T>(v);
                    } else if constexpr(std::numeric_limits<T>::is_signed){
                        printIntSigned<T>(v);
                    } else {
                        printIntUnsigned<T>(v);
                    }
                } else if constexpr(std::is_pointer<T>::value){
                    printIntHex<T>(v);
                } else if constexpr(std::is_same<T,float>::value){
                    printFloat(v);
                } else if constexpr(std::is_same<T,double>::value){
                    printDouble(v);
                } else {
                    constexpr bool has_toString = requires(const T& t) {
                        t.toString();
                    };
                    if constexpr(has_toString){
                        putString(v.toString());
                    } else {
                        static_assert(has_toString,"Can't print; Value must have to_string method or be of int, pointer of float types");
                    }
                }
                return *this;
            }
        private:
            void putString(const char* s);
            void putChar(const char c, uint32_t yOff, uint32_t xOff);
            FrameBuffer fb;
            Util::Vector2<uint32_t> pos;
            PSF1_FONT font;
            uint32_t color = 0xffffffff;
            template<typename T>
            void printIntUnsigned(const T value){
                static_assert(std::is_same_v<T, uint64_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint8_t>);

            }
            template<typename T>
            void printIntSigned(const T value){
                static_assert(std::is_same_v<T, int64_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int8_t>);
            }
            template<typename T>
            void printIntHex(const T value){
                constexpr uint64_t size = sizeof(T);
                putString("0x");
            }
            void printFloat(const float f);
            void printDouble(const double d);
    };
}