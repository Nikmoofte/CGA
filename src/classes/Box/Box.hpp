#ifndef BOX_HPP
#define BOX_HPP

#include <Windows.h>
#include <string>
#include <glm\glm.hpp>

class Box
{
public:
    Box(long top, long left, long right, long bottom) : box({top, left, right, bottom}), dir(0.0f) {}
    Box() : box({0, 0, 0, 0}), dir(0.0f) {}

    void loadBitMap(HBITMAP bitmap);
    void resize(RECT newRect);
    void createDC(HDC dc);
    void releaseDC();

    glm::vec2 colide(RECT* borders);
    void move();

    RECT getRedrawRect();
    RECT getRedrawRect(const glm::vec2 movement);

    void draw(HDC hdc);
    void accelirateDir(const glm::vec2& accsDir);
    void accelerateToPoint(const glm::vec2 &point, float coef);
    const glm::vec2& getDir();
private:
    void changeDir(const glm::vec2& newDir);
    
    constexpr static float frictionCoef = 0.95f;

    HDC dc;
    HBITMAP bitmap = nullptr;
    HBITMAP prevBitmap = nullptr;

    glm::vec2 dir;
    RECT box;
};

#endif