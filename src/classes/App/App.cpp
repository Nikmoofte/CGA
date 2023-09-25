#include "App.hpp"

#include <stdexcept>
#include <memory>
#include <chrono>
#include "PngLoader/PngLoader.hpp"

App::App() : className{ "Lab_1_WNDCLASS" }, lable{ "lab 1" }, box()
{
    box.resize({0, 0, 50, 50});
    try
    {
        registerClass(className.c_str());

        wndHandle = CreateWindowEx(
        0,                              // Optional window styles.
        className.c_str(),              // Window class
        lable.c_str(),                  // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        // Size and position
        0, 0, 1920, 1080,
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
    PngLoader loader;
    auto hBM = loader(filePath);

    box.loadBitMap(hBM);

}

int App::run()
{
    MSG msg{};
    ShowWindow(wndHandle, SW_SHOW);
    UpdateWindow(wndHandle);

    auto timerStart = std::chrono::system_clock::now();
    //DEVMODEW devMode;
    //EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    //auto refrashRate = devMode.dmDisplayFrequency;
    auto refrashRate = 60;
    while(msg.message != WM_QUIT)
    {
        if(PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
            DispatchMessageW(&msg);
        
        std::chrono::duration<float> dur = std::chrono::system_clock::now() - timerStart;
        if(dur.count() > 1.0f / refrashRate)
        {
            glm::vec2 dir{right - left, up - down};
            RECT screenSpace{0, 0, appWidht, appHeight};
            
            box.accelirateDir(dir * 1.2f);

            box.colide(&screenSpace);
            box.move();

            auto memDC = CreateCompatibleDC(dc);
            auto memBM = CreateCompatibleBitmap(dc, appWidht, appHeight);
            auto prev = (HBITMAP)SelectObject(memDC, memBM);

            FillRect(memDC, &screenSpace, (HBRUSH)GetStockObject(WHITE_BRUSH));
            box.draw(memDC);

            BitBlt(dc, 0, 0, appWidht, appHeight, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, prev);
            DeleteObject(memBM);
            DeleteDC(memDC);

            timerStart = std::chrono::system_clock::now();  
        }
    }

    return (int)(msg.wParam);
}

LRESULT App::appProg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    App* pApp;
    if(uMsg == WM_NCCREATE)
    {
        pApp = (App*)(((CREATESTRUCTW*)lParam)->lpCreateParams);
        SetLastError(0);
        if(!SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pApp))
        {
            if(GetLastError() != 0)
                return false;
        }
    }
    else
        pApp = (App*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    
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
            box.createDC(dc);
        return 0;
        /*case WM_PAINT:  
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HDC memDC = CreateCompatibleDC(hdc);

            auto buffer = CreateCompatibleBitmap(hdc, appWidht, appHeight);

            auto oldBuffer = (HBITMAP)SelectObject(memDC, buffer);

            FillRect(memDC, &ps.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));
            auto oldBrush = (HBRUSH)SelectObject(memDC, (HBRUSH)GetStockObject(BLACK_BRUSH));
            box.draw(memDC);

            BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom, memDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

            SelectObject(memDC, oldBuffer);
            DeleteObject(buffer);
            DeleteDC(memDC);

            SelectObject(hdc, oldBrush);
            EndPaint(hwnd, &ps);
            return 0;
        }
        break;*/
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_UP:
                up = true;
                break;
            case VK_DOWN:
                down = true;
                break;
            case VK_LEFT:
                left = true;
                break;
            case VK_RIGHT:
                right = true;
                break;
            }

            return 0;
        }
        case WM_KEYUP:
        {
            switch (wParam)
            {
            case VK_UP:
                up = false;
                break;
            case VK_DOWN:
                down = false;
                break;
            case VK_LEFT:
                left = false;
                break;
            case VK_RIGHT:
                right = false;
                break;
            }

            return 0;
        }
        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = 100;
            lpMMI->ptMinTrackSize.y = 100;
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
            box.releaseDC();
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZING:
        {
            RECT* pRect = (RECT*)lParam;
            appWidht = pRect->right - pRect->left - 15; 
            appHeight = pRect->bottom - pRect->top - 39;
            return true;
        }
        case WM_SIZE:
        {
            appWidht = LOWORD(lParam);
            appHeight = HIWORD(lParam);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
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
