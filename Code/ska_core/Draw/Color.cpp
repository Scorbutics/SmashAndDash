#include "Color.h"

namespace ska {
    template<>
    ska::Color ska::Color::operator+(const ska::Color& cadder) {
        ska::Color result;
        result.r = r + cadder.r;
        result.g = g + cadder.g;
        result.b = b + cadder.b;
        result.a = a + cadder.a;
        return result;
    }
}
