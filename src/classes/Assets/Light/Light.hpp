#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

namespace Assets
{
    struct Light
    {
        Light() = default;
        Light(const glm::vec3& pos, const glm::vec3& color) : pos(pos), color(color) {}

        glm::vec3 pos{0.0f};
        glm::vec3 color{1.0f};
    };
}
#endif