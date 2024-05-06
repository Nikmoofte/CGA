#ifndef PERPDOT_HPP
#define PERPDOT_HPP

#include <glm/glm.hpp>

inline float perpDot(const glm::vec2& a, const glm::vec2& b) {
    return a.x * b.y - a.y * b.x;
}

#endif