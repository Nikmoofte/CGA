#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <glm/glm.hpp>
#include <array>

namespace Engine
{
    class Triangle
    {
    public:
        Triangle(size_t ind, std::array<size_t,3>&& vertInds, std::array<glm::vec2,3>&& verts);
        Triangle(size_t ind, std::array<size_t,3>& vertInds, std::array<glm::vec2,3>& verts);
        Triangle(size_t ind, const std::array<size_t, 3>& vertInds);
        Triangle() = default;
        Triangle(Triangle &&) = default;
        Triangle(const Triangle &) = default;
        Triangle& operator=(Triangle &&) = default;
        Triangle& operator=(const Triangle &) = default;

        const std::array<size_t, 3>& getVertInds() const;
        inline float getDenom() const {return denom;};
        glm::vec3 getBarycentric(const glm::vec2& p);
        bool isClipped() const { return clipped; };
        void Clip() { clipped = true; };

        bool isBackFace() const;
    private:
        size_t id{};
        float d00;
        float d01;
        float d11;
        float denom;
        bool clipped = false;
        std::array<glm::vec2, 3> verts{};
        std::array<size_t, 3> vertInds{};
    };
}

#endif