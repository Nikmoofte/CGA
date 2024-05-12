#include "Triangle.hpp"

constexpr float oneThird = 1.0f / 3.0f; 

namespace Engine
{
    Triangle::Triangle(size_t ind, std::array<size_t,3>&& vertInds, std::array<Assets::Vertex, 3>&& verts)
    : id(ind), vertInds(std::move(vertInds)), verts(std::move(verts))
    {
        glm::vec2 v0 = this->verts[1].projectedPosition - this->verts[0].projectedPosition, 
                  v1 = this->verts[2].projectedPosition - this->verts[0].projectedPosition;
        d00 = glm::dot(v0, v0);
        d01 = glm::dot(v0, v1);
        d11 = glm::dot(v1, v1);
        denom = 1 / (d00 * d11 - d01 * d01);
    }

    Triangle::Triangle(size_t ind, std::array<size_t, 3> &vertInds, std::array<Assets::Vertex, 3> &verts)
    : id(ind), vertInds(vertInds), verts(verts)
    {
        glm::vec2 v0 = verts[1].projectedPosition - verts[0].projectedPosition, 
                  v1 = verts[2].projectedPosition - verts[0].projectedPosition;
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
        glm::vec2 v0 = verts[1].projectedPosition - verts[0].projectedPosition, 
                  v1 = verts[2].projectedPosition - verts[0].projectedPosition, 
                  v2 = p - glm::vec2(verts[0].projectedPosition);
        float d20 = glm::dot(v2, v0);
        float d21 = glm::dot(v2, v1);
        float v = (d11 * d20 - d01 * d21) * denom;
        float w = (d00 * d21 - d01 * d20) * denom;
        float u = 1.0f - v - w;
        return {u, v, w};
    }

    bool Triangle::isBackFace(const glm::vec3& camPos) const
    {
        glm::vec3 norm = (verts[0].normal + verts[1].normal + verts[2].normal) * oneThird; 
        return glm::dot(norm, camPos - verts[0].position) < 0;
    }
}
