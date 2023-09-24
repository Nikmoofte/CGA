#include "Box.hpp"

#include <algorithm>

void Box::createDC(HDC dc)
{
    this->dc = CreateCompatibleDC(dc);
    bitmap = CreateCompatibleBitmap(dc, box.right, box.bottom);
    
    prevBitmap = (HBITMAP)SelectObject(this->dc, bitmap);
    
    // auto oldBrush = (HBRUSH)SelectObject(this->dc, (HBRUSH)GetStockObject(BLACK_BRUSH));

}

void Box::releaseDC()
{
    SelectObject(dc, prevBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
}

void Box::move()
{
    glm::vec2 movement = dir;
    movement.y = -movement.y;

    box.top += (int)movement.y;
    box.bottom += (int)movement.y;

    box.left += (int)movement.x;
    box.right += (int)movement.x;

    dir *= frictionCoef;
    
    if(abs(dir.x) < 1)
        dir.x = 0;
    if(abs(dir.y) < 1)
        dir.y = 0;
}

RECT Box::getRedrawRect()
{
    glm::vec2 movement = dir;
    movement.y = -movement.y;

    RECT redrawRect = box;

    redrawRect.right += (int)std::max(0.0f, movement.x);
    redrawRect.left += (int)std::min(0.0f, movement.x);
    
    redrawRect.top += (int)std::min(0.0f, movement.y);
    redrawRect.bottom += (int)std::max(0.0f, movement.y);
    
    return redrawRect;
}

RECT Box::getRedrawRect(const glm::vec2 movement)
{
    RECT redrawRect = box;

    redrawRect.right += std::max(0.0f, movement.x);
    redrawRect.left += std::min(0.0f, movement.x);
    
    redrawRect.top += std::min(0.0f, movement.y);
    redrawRect.bottom += std::max(0.0f, movement.y);

    return redrawRect;
}

void Box::draw(HDC hdc)
{
    BitBlt(hdc, box.left, box.top, box.right, box.bottom, dc, 0, 0, SRCCOPY);
}

void Box::changeDir(const glm::vec2& newDir)
{
    dir = newDir;
    
}

void Box::accelirateDir(const glm::vec2& accsDir)
{
    dir += accsDir;
    
}

const glm::vec2 &Box::getDir()
{
    return dir;
}
