#include "App.hpp"

#include <stdexcept>
#include <memory>
#include <iostream>

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
        wc.hbrBackground = HBRUSH(GetStockObject(WHITE_BRUSH));
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

    while(GetMessageW(&msg, nullptr, 0, 0))
    {
        DispatchMessageW(&msg);
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
            SelectObject(hdc, HBRUSH(GetStockObject(BLACK_BRUSH)));
            // All painting occurs here, between BeginPaint and EndPaint.


            box.draw(hdc);



            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_KEYDOWN:
        {
            glm::vec2 dir{0.0f};

            if(GetKeyState(VK_UP) < 0)
                dir.y += 1.0f;
            if(GetKeyState(VK_DOWN) < 0)
                dir.y -= 1.0f;
            if(GetKeyState(VK_LEFT) < 0)
                dir.x -= 1.0f;
            if(GetKeyState(VK_RIGHT) < 0)
                dir.x += 1.0f;
                
            box.changeDir(dir);
            InvalidateRect(hwnd, &box.move(5), true);
            UpdateWindow(wndHandle);
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
