#ifndef APP_HPP
#define APP_HPP

#include <Windows.h>

#include <cstdint>
#include <string>
#include <exception>
#include <chrono>
#include <gdiplus.h>

#include "Object/Object.hpp"


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
    inline void draw();

    inline void Brezenhem(Gdiplus::Bitmap&, glm::ivec2, glm::ivec2, const Gdiplus::Color&);

    std::chrono::system_clock::time_point appStart = std::chrono::system_clock::now();
    HWND wndHandle{};
    std::string className;
    std::string lable;
    long appWidht, appHeight;
    
    ULONG_PTR gdiplusToken;
    Object obj;
    HDC dc;
};

#endif