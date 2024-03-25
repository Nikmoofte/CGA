#ifndef COLOR4B_HPP
#define COLOR4B_HPP

#include <cinttypes>
#include <glm/glm.hpp>
#include <algorithm>


namespace Assets
{
    class Color4b
    {
    public:
        Color4b() = default;
        Color4b(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
        inline void fromVec(glm::vec4 color)
        {
            color *= 255.0f;
            r = std::clamp<uint8_t>(color.g, 0, 255);
            g = std::clamp<uint8_t>(color.b, 0, 255);
            b = std::clamp<uint8_t>(color.r, 0, 255);
            a = std::clamp<uint8_t>(color.a, 0, 255);
        }
        inline glm::vec4 toVec()
        {
            return glm::vec4((float)r, (float)g, (float)b, (float)a) / 255.0f;
        }

        inline Color4b operator *(float val) const
        {
            return Color4b(uint8_t(val * r), uint8_t(val * g), uint8_t(val * b));
        }

        inline Color4b operator +(const Color4b& color) const
        {
            return Color4b(r + color.r, g + color.g, b + color.b);
        }

        inline Color4b& operator +=(const Color4b& color)
        {
            r += color.r;
            g += color.g;
            b += color.b;
            a = 1;

            return *this;
        }

        inline Color4b operator /(float val) const
        {
            float fInv = 1.0f / val;
            return Color4b(uint8_t(r * fInv), uint8_t(g * fInv), uint8_t(b * fInv));
        }

    private:
        uint8_t r{};
        uint8_t g{};
        uint8_t b{};
        uint8_t a{};
    };
}
#endif