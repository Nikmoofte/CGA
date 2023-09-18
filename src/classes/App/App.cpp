#include "App.hpp"

#include <stdexcept>
#include <memory>
#include <chrono>

App::App() : box({0, 0, 25, 25})
{
    try
    {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };

        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.lpfnWndProc   = App::appProg;
        wc.hInstance     = GetModuleHandleW(nullptr);
        wc.lpszClassName = className.c_str();
        wc.hbrBackground = NULL;
        wc.lpszMenuName = nullptr;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        
        if(!RegisterClassExW(&wc))
            throw std::runtime_error{"Failed to create window class!"};


        wndHandle = CreateWindowExW(
        0,                              // Optional window styles.
        className.c_str(),              // Window class
        lable.c_str(),                  // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,                        // Parent window    
        nullptr,                        // Menu
        wc.hInstance,                   // Instance handle
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

int App::run()
{
    MSG msg{};
    ShowWindow(wndHandle, SW_SHOW);
    UpdateWindow(wndHandle);

    auto timerStart = std::chrono::system_clock::now();
    DEVMODEW devMode;
    EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    auto refrashRate = devMode.dmDisplayFrequency;
    while(msg.message != WM_QUIT)
    {
        if(PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
            DispatchMessageW(&msg);
        
        std::chrono::duration<float> dur = std::chrono::system_clock::now() - timerStart;
        if(dur.count() > 1.0f / refrashRate)
        {
            glm::vec2 dir{right - left, up - down};

            box.changeDir(dir);
            auto redrawRect = box.move(baseSpeed);
            if(redrawRect.top < 0 || 
                redrawRect.left < 0 || 
                redrawRect.right > appWidht || 
                redrawRect.bottom > appHeight)
            {
                box.move(-baseSpeed);
            }
            else
            {
                InvalidateRect(wndHandle, &redrawRect, true);
                UpdateWindow(wndHandle);
            }
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
        case WM_PAINT:  
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
        break;
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
            // if (MessageBoxW(hwnd, L"Really quit?", L"Confirm", MB_OKCANCEL || MB_ICONQUESTION) == IDOK)
            // {
            //     DestroyWindow(hwnd);
            // }
            DestroyWindow(hwnd);
            return 0;
        }
        case WM_DESTROY:
        {
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
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
