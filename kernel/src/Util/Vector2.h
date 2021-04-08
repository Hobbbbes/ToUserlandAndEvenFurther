#pragma once

namespace Util{
    template<typename T>
    class Vector2{
        public:
            T x;
            T y;
            Vector2(T x, T y):x(x),y(y){}
            Vector2(){};
    };
}