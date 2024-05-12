#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <glm/glm.hpp>
#include <Assets/Vertex.hpp>
#include <array>

namespace Engine
{
    class Triangle
    {
    public:
        Triangle(size_t ind, std::array<size_t,3>&& vertInds, std::array<Assets::Vertex, 3>&& verts);
        Triangle(size_t ind, std::array<size_t,3>& vertInds, std::array<Assets::Vertex, 3>& verts);
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
        void setMaterialId(int id) { materialId = id; };
        int getMaterialId() const { return materialId; };
        
        size_t getId() const { return id; } 

        bool isBackFace(const glm::vec3& camPos) const;
    private:
        size_t id{};
        float d00;
        float d01;
        float d11;
        float denom;
        bool clipped = false;
        int materialId = 0;
        glm::vec3 bary{0.0f};
        std::array<Assets::Vertex, 3> verts{};
        std::array<glm::vec3, 3> vertsInitial{};
        std::array<size_t, 3> vertInds{};
    };
}

#endif