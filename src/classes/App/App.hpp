#ifndef APP_HPP
#define APP_HPP

#include <Windows.h>

#include <cstdint>
#include <string>
#include <exception>

#include "Box/Box.hpp"

// #define SHOW_COLLISION


class App
{
public:
    explicit App();
    explicit App(std::string filePath);

    int run();
private:
    static LRESULT CALLBACK appProg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    inline void registerClass(const CHAR* className);
    inline void mouseHandle(int xPos, int yPos);
    inline void draw();

    HWND wndHandle{};
    std::string className;
    std::string lable;
    long appWidht, appHeight;
    
    HDC dc;
    
    Box box;
    bool up = false, down = false;
    bool left = false, right = false;
    bool lButtonDown = false;

};

#endif