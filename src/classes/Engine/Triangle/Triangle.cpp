#include "Triangle.hpp"



Engine::Triangle::Triangle(size_t ind, std::array<glm::ivec2,3>&& screenSpaceVerts, std::array<uint32_t,3>&& vertInds)
: id(ind), screenSpaceVerts(screenSpaceVerts), vertInds(vertInds)
{

}

Engine::Triangle::Triangle(size_t ind, const std::array<glm::ivec2, 3> &screenSpaceVerts, const std::array<uint32_t, 3> &vertInds)
: id(ind), screenSpaceVerts(screenSpaceVerts), vertInds(vertInds)
{
    
}
