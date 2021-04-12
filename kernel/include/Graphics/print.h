#pragma once
#include "../PreBoot/Framebuffer.h"
#include "../PreBoot/PSF1_Font.h"
#include "../Util/Vector2.h"
#include <type_traits>
#include <limits>
namespace Graphics{
    constexpr uint32_t Red = 0x00ff0000;
    class TextDrawer{
        public:
            TextDrawer(FrameBuffer &fb, PSF1_FONT &font);
            TextDrawer(){}
            TextDrawer& setColor(const uint32_t c);
            enum class Print_Specifier{
                Hex,Standart
            };
            template<typename T,Print_Specifier S = Print_Specifier::Standart>
            TextDrawer& print(const T v){
                if constexpr(S == Print_Specifier::Hex){
                        printIntHex<T>(v);
                }
                if constexpr((std::is_same<T, const char*>::value || std::is_same<T,char*>::value)){
                    putString(v);
                } else if constexpr(std::is_same<T, uint64_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, uint16_t>::value || std::is_same<T, uint8_t>::value
                || std::is_same<T, int64_t>::value || std::is_same<T, int32_t>::value || std::is_same<T, int16_t>::value|| std::is_same<T, int8_t>::value){

                    if constexpr(std::numeric_limits<T>::is_signed){
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
                } else if constexpr(std::is_same<T,bool>::value) {
                    if(v){putString("True");}
                    else{putString("False");} 
                } else if (S == Print_Specifier::Standart) {
                    constexpr bool has_toString = requires(const T& t) {
                        t.toString();
                    };
                    if constexpr(has_toString){
                        putString(v.toString());
                    } else {
                        static_assert(has_toString || S != Print_Specifier::Standart,"Can't print; Value must have to_string method or be of int, pointer of float types");
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
            void printIntUnsigned(const T v){
                static_assert(std::is_same<T, uint64_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, uint16_t>::value || std::is_same<T, uint8_t>::value);
                T value = v;
                uint8_t index = 0;
                char s[21];
                while(value / 10 > 0){
                    uint8_t remainder = value % 10;
                    value /= 10;
                    s[19 - index] = remainder + '0';
                    index++;
                }
                uint8_t remainder = value % 10;
                s[19 - index] = remainder + '0';
                s[20] = '\0';
                putString(reinterpret_cast<const char*>(reinterpret_cast<uint64_t>(s) + (19-index)));
            }
            template<typename T>
            void printIntSigned(const T v){
                static_assert(std::is_same<T, int64_t>::value || std::is_same<T, int32_t>::value || std::is_same<T, int16_t>::value || std::is_same<T, int8_t>::value);
                bool negativ;
                T value = v;
                if(value < 0){
                    negativ = true;
                    value *= -1;
                }
                uint8_t index = 0;
                char s[22];
                while(value / 10 > 0){
                    uint8_t remainder = value % 10;
                    value /= 10;
                    s[20 - index] = remainder + '0';
                    index++;
                }
                uint8_t remainder = value % 10;
                s[20 - index] = remainder + '0';
                s[21] = '\0';
                if(negativ){
                    index++;
                    s[20 - index] = '-';
                }
                putString(reinterpret_cast<const char*>(reinterpret_cast<uint64_t>(s) + (20-index)));
            }
            template<typename T>
            void printIntHex(const T value){
                constexpr uint64_t size = sizeof(T);
                putString("0x");
                const uint8_t* valPtr = reinterpret_cast<const uint8_t*>(&value);
                for (uint8_t i = 0; i < size; i++){
                    uint8_t* ptr = ((uint8_t*)valPtr + size - i -1);
                    uint8_t tmp = ((*ptr & 0xF0) >> 4);
                    char s[3]; s[2] = '\0';
                    s[0] = tmp + (tmp > 9 ? 55 : '0');
                    tmp = ((*ptr & 0x0F));
                    s[1] = tmp + (tmp > 9 ? 55 : '0');
                    putString(s);
                }
            }
            void printFloat(const float f);
            void printDouble(const double d);
    };
    extern TextDrawer KernelDrawer;
}