#include "Color.h"

namespace ska {
    template<>
    ska::Color ska::Color::operator+(const ska::Color& cadder) {
        return Color(r + cadder.r, g + cadder.g, b + cadder.b, a + cadder.a);
    }
}
