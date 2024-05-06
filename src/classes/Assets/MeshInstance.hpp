#ifndef MESHINSTANCE_HPP
#define MESHINSTANCE_HPP

#include <glm/mat4x4.hpp>
namespace Assets
{
    struct MeshInstance
    {
        int meshId;
        glm::mat4 model;
    };
}

#endif