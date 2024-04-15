#include "Triangle.hpp"

constexpr float oneThird = 1.0f / 3.0f; 

namespace Engine
{
    Triangle::Triangle(size_t ind, std::array<size_t,3>&& vertInds, std::array<glm::vec2, 3>&& verts)
    : id(ind), vertInds(std::move(vertInds)), verts(std::move(verts))
    {
        glm::vec2 v0 = this->verts[1] - this->verts[0], v1 = this->verts[2] - this->verts[0];
        d00 = glm::dot(v0, v0);
        d01 = glm::dot(v0, v1);
        d11 = glm::dot(v1, v1);
        denom = 1 / (d00 * d11 - d01 * d01);
    }

    Triangle::Triangle(size_t ind, std::array<size_t, 3> &vertInds, std::array<glm::vec2, 3> &verts)
    : id(ind), vertInds(vertInds), verts(verts)
    {
        glm::vec2 v0 = verts[1] - verts[0], v1 = verts[2] - verts[0];
        d00 = glm::dot(v0, v0);
        d01 = glm::dot(v0, v1);
        d11 = glm::dot(v1, v1);
        denom = 1 / (d00 * d11 - d01 * d01);

    }

    const std::array<size_t, 3> &Triangle::getVertInds() const
    {
        return vertInds;
    }

    glm::vec3 Triangle::getBarycentric(const glm::vec2 &p)
    {
        glm::vec2 v0 = verts[1] - verts[0], v1 = verts[2] - verts[0], v2 = p - verts[0];
        float d20 = glm::dot(v2, v0);
        float d21 = glm::dot(v2, v1);
        float v = (d11 * d20 - d01 * d21) * denom;
        float w = (d00 * d21 - d01 * d20) * denom;
        float u = 1.0f - v - w;
        return {v, w, u};
    }

    bool Triangle::isBackFace() const
    {
        return false;
    }
}
