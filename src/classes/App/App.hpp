#ifndef APP_HPP
#define APP_HPP

#include <Windows.h>

#include <cstdint>
#include <string>
#include <exception>

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

    HWND wndHandle{};
    std::string className;
    std::string lable;
    long appWidht, appHeight;
    
    HDC dc;
};

#endif