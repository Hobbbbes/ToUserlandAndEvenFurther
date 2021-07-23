#pragma once
namespace Util{
    template<typename T, typename U>
    class Pair{
        public:
            T first;
            U second;
            Pair(){}
            Pair(T& first, U& second) : first(first), second(second) {}
            Pair(T first, U second) : first(first), second(second) {}
    };
}