#ifndef APP_HPP
#define APP_HPP

#include <Windows.h>

#include <cstdint>
#include <string>
#include <exception>

#include "Box/Box.hpp"

class App
{
public:
    explicit App();

    int run();

private:
    static LRESULT CALLBACK appProg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND wndHandle{};
    std::wstring className{ L"Lab_1_WNDCLASS" };
    std::wstring lable{ L"lab 1" };
    uint32_t appWidht, appHeight;

    const int baseSpeed = 1;
    
    Box box;
    bool up = false, down = false;
    bool left = false, right = false;

};

#endif