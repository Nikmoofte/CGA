#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <glm/glm.hpp>
#include <array>

namespace Engine
{
    class Triangle
    {
    public:
        Triangle(size_t ind, std::array<glm::ivec2, 3>&& screenSpaceVerts, std::array<uint32_t, 3>&& vertInds);
        Triangle(size_t ind, const std::array<glm::ivec2, 3>& screenSpaceVerts, const std::array<uint32_t, 3>& vertInds);
    private:
        size_t id;
        std::array<glm::ivec2, 3> screenSpaceVerts;
        std::array<uint32_t, 3> vertInds;
    };
}

#endif