#ifndef BOX_HPP
#define BOX_HPP

#include <Windows.h>
#include <glm\glm.hpp>

class Box
{
public:
    Box(long top, long left, long right, long bottom) : box({top, left, right, bottom}), dir(0.0f) {}
    

    void createDC(HDC dc);
    void releaseDC();
    void move();
    RECT getRedrawRect();
    RECT getRedrawRect(const glm::vec2 movement);
    void draw(HDC hdc);
    void changeDir(const glm::vec2& newDir);
    void accelirateDir(const glm::vec2& accsDir);
    const glm::vec2& getDir();
private:
    constexpr static float frictionCoef = 0.95f;

    HDC dc;
    HBITMAP bitmap, prevBitmap;
    glm::vec2 dir;
    RECT box;
};

#endif