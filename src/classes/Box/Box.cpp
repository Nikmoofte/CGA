#include "Box.hpp"

#include <algorithm>
#include <stdexcept>

void Box::loadBitMap(HBITMAP bitmap)
{
    DeleteObject(this->bitmap);
    this->bitmap = bitmap;
    prevBitmap = (HBITMAP)SelectObject(dc, this->bitmap);

    BITMAP bm;
    GetObject(bitmap, sizeof(bm), &bm);
    RECT newSize{0, 0, bm.bmWidth, bm.bmHeight};

    resize(newSize);
}

void Box::resize(RECT newRect)
{
    box = newRect;
}

void Box::createDC(HDC dc)
{
    this->dc = CreateCompatibleDC(dc);
    if(!bitmap)
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

glm::vec2 Box::colide(RECT* borders)
{
    auto redrawRect = getRedrawRect();
    glm::vec2 conterDir{(redrawRect.left < borders->left) - (redrawRect.right > borders->right),
                        (redrawRect.bottom > borders->bottom) - (redrawRect.top < borders->top)};
    if(conterDir == glm::vec2(0.0f))
        return conterDir;
    return glm::normalize(conterDir);
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
    
    return getRedrawRect(movement);
}

RECT Box::getRedrawRect(const glm::vec2 movement)
{
    RECT redrawRect = box;
    int Plus = movement.length();

    redrawRect.right += std::max(0.0f, movement.x);
    redrawRect.left += std::min(0.0f, movement.x);
    
    redrawRect.top += std::min(0.0f, movement.y);
    redrawRect.bottom += std::max(0.0f, movement.y);

    return redrawRect;
}

void Box::draw(HDC hdc)
{
    
    BLENDFUNCTION blend{0};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = BYTE(255);
    blend.AlphaFormat = AC_SRC_ALPHA;

    RECT src{0, 0, box.right - box.left, box.bottom - box.top};

    AlphaBlend(hdc, box.left, box.top, src.right, src.bottom, dc, src.left, src.top, src.right, src.bottom, blend);
    //BitBlt(hdc, box.left, box.top, box.right, box.bottom, dc, src.left, src.top, SRCAND);
}

void Box::changeDir(const glm::vec2& newDir)
{
    dir = newDir;
    
}

void Box::accelirateDir(const glm::vec2& accsDir)
{
    dir += accsDir;
    
}

void Box::accelerateToPoint(const glm::vec2 &point, float coef)
{
    SIZE boxSize{box.right - box.left, box.bottom - box.top};
    glm::vec2 center{
        box.left + (boxSize.cx >> 1),
        box.top + (boxSize.cy >> 1)
        };

    auto dir = point - center;
    dir = dir == glm::vec2(0.0f) ? dir : glm::normalize(dir);
    dir.y = -dir.y; 

    accelirateDir(dir * coef);
}

const glm::vec2 &Box::getDir()
{
    return dir;
}
