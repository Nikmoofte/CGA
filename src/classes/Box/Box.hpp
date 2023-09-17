#ifndef BOX_HPP
#define BOX_HPP

#include <Windows.h>
#include <glm\glm.hpp>

class Box
{
public:
    Box(long top, long left, long right, long bottom) : box({top, left, right, bottom}), dir(0.0f) {}

    RECT move(long speed);
    void draw(HDC hdc);
    void changeDir(const glm::vec2& newDir);
    const glm::vec2& getDir();
private:
    glm::vec2 dir;
    RECT box;
};

#endif