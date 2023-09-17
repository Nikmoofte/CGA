#include "Box.hpp"

#include <algorithm>

RECT Box::move(long speed)
{
    RECT redrawRect = box;
    glm::vec2 movement = dir * (float)speed;
    movement.y = -movement.y;

    box.top += movement.y;
    box.bottom += movement.y;

    box.left += movement.x;
    box.right += movement.x;
    
    redrawRect.right += std::max(0.0f, movement.x);
    redrawRect.left += std::min(0.0f, movement.x);
    
    redrawRect.top += std::min(0.0f, movement.y);
    redrawRect.bottom += std::max(0.0f, movement.y);
    
    return redrawRect;
}

void Box::draw(HDC hdc)
{
    Rectangle(hdc, box.left, box.top, box.right, box.bottom);
}

void Box::changeDir(const glm::vec2& newDir)
{
    dir = newDir;
    
}

const glm::vec2 &Box::getDir()
{
    return dir;
}
