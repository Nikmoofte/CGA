#ifndef APP_HPP
#define APP_HPP

#include <Windows.h>
#include <cstdint>
#include <string>
#include <exception>
#include <chrono>
#include <gdiplus.h>
#include <thread>

#include <glm/glm.hpp>

#include "Object/Object.hpp"
#include "Camera/Camera.hpp"


class App
{
public:
    explicit App();
    explicit App(const std::string& filePath);

    int run();
private:
    static LRESULT CALLBACK appProg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    inline void registerClass(const CHAR* className);
    inline void draw();

    inline void Brezenhem(std::vector<DWORD32>&, glm::ivec2, glm::ivec2, const Gdiplus::Color&);


    std::chrono::system_clock::time_point appStart = std::chrono::system_clock::now();
    HWND wndHandle{};
    std::string className;
    std::string lable;
    size_t appWidht, appHeight;

    Camera camera{glm::vec3(-10.0f, 0.0f, 0.0f), 1920, 1080};
    
    std::vector<std::thread> threads{std::thread::hardware_concurrency()};

    bool rmdown = false;
    Object obj;
    HDC dc;
};

#endif