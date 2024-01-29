#include "App.hpp"

#include <stdexcept>
#include <memory>
#include <chrono>
#include <Windowsx.h>
#include <glm/glm.hpp>

#include "ObjParser/ObjParser.hpp"

App::App() : className{ "Lab_1_WNDCLASS" }, lable{ "Lab 1" }
{
    try
    {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;

        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
        registerClass(className.c_str());

        wndHandle = CreateWindowEx(
        0,                              // Optional window styles.
        className.c_str(),              // Window class
        lable.c_str(),                  // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,                        // Parent window    
        nullptr,                        // Menu
        GetModuleHandle(nullptr),      // Instance handle
        this                            // Additional application data
        );

        if (!wndHandle)
            throw std::runtime_error{"Failed to create window!"};

    }
    catch(const std::exception& e)
    {
        MessageBox(nullptr, e.what(), "ERROR", MB_OK | MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }
    
}

App::App(std::string filePath) : App()
{
    //obj = ObjParser{}(filePath);
}

int App::run()
{
    MSG msg{};
    ShowWindow(wndHandle, SW_SHOW);
    UpdateWindow(wndHandle);

    auto timerStart = std::chrono::system_clock::now();
    DEVMODEA devMode;
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    auto refrashRate = devMode.dmDisplayFrequency;
    while(msg.message != WM_QUIT)
    {
        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            DispatchMessage(&msg);
        
        std::chrono::duration<float> dur = std::chrono::system_clock::now() - timerStart;
        if(dur.count() > 1.0f / refrashRate)
        {
            draw();

            timerStart = std::chrono::system_clock::now();  
        }
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return (int)(msg.wParam);
}

LRESULT App::appProg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    App* pApp;
    if(uMsg == WM_NCCREATE)
    {
        pApp = (App*)(((CREATESTRUCTW*)lParam)->lpCreateParams);
        SetLastError(0);
        if(!SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pApp))
        {
            if(GetLastError() != 0)
                return false;
        }
    }
    else
        pApp = (App*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    if(pApp)
    {
        pApp->wndHandle = hwnd;
        return pApp->WindowProc(hwnd, uMsg, wParam, lParam);

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT App::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
            dc = GetDC(hwnd);
        return 0;
        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = 200L;
            lpMMI->ptMinTrackSize.y = 200L;
            return 0;
        }
        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }
        case WM_DESTROY:
        {
            ReleaseDC(hwnd, dc);
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE:
        {
            appWidht = LOWORD(lParam);
            appHeight = HIWORD(lParam);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

inline void App::registerClass(const CHAR* className)
{
     WNDCLASSEX wc = { sizeof(WNDCLASSEX) };

        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.lpfnWndProc   = App::appProg;
        wc.hInstance     = GetModuleHandleW(nullptr);
        wc.lpszClassName = className;
        wc.hbrBackground = NULL;
        wc.lpszMenuName = nullptr;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        
        if(!RegisterClassEx(&wc))
            throw std::runtime_error{"Failed to create window class!"};
}

inline void App::draw()
{        
    //DRAW
        Gdiplus::Graphics graphics(dc);

        Gdiplus::Bitmap bitmap(appWidht, appHeight, PixelFormat32bppRGB);
        std::chrono::duration<double> time = std::chrono::system_clock::now() - appStart;
        Brezenhem(bitmap, { appWidht / 2 - sin(time.count()) * 100, appHeight / 2 }, { appWidht / 2 + sin(time.count()) * 100, appHeight / 2 }, Gdiplus::Color{ 255, 0, 0 });

        graphics.DrawImage(&bitmap, 0, 0);


    //STOP DRAW
}

inline void App::Brezenhem(Gdiplus::Bitmap& bitmap, glm::ivec2 start, glm::ivec2 end, const Gdiplus::Color& color)
{
    int dx = abs(end.x - start.x);
    int dy = abs(end.y - start.y);
    int sx = (end.x < start.x) ? -1 : 1;
    int sy = (end.y < start.y) ? -1 : 1;
    int err = dx - dy;

    while (start.x != end.x || start.y != end.y)
    {
        bitmap.SetPixel(start.x, start.y, color);

        int e2 = err << 1;
        if (e2 > -dy)
        {
            err -= dy;
            start.x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            start.y += sy;
        }
    }
}
