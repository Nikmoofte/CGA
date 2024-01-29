#include "App.hpp"

#include <stdexcept>
#include <memory>
#include <chrono>
#include <Windowsx.h>
#include <glm/glm.hpp>
#include <mutex>

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
    obj = ObjParser{}(filePath);
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

        camera.KeyboardControl();
        if(rmdown)
            camera.MouseControl();
        
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
        case WM_KEYDOWN:
        {
            switch(wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    return 0;
            }
            return 0;
        }
        case WM_RBUTTONDOWN:
        {   
            rmdown = true;
            return 0;
        }
        case WM_RBUTTONUP:
        {
            rmdown = false;
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
            camera.ChangeView(appWidht, appHeight);
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
    auto view = camera.GetViewMat();
    auto proj = camera.GetProjMat();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = camera.GetViewportMat() * proj * view * model;

    size_t facesNum = obj.faces.size();
    size_t threadFacesNum = facesNum / threads.size();
    for(int i = 0; i < threads.size(); ++i)
    {
        threads[i] = std::thread([&, i, facesNum, threadFacesNum]()
        {
            for(size_t j = i * threadFacesNum; j < (i + 1) * threadFacesNum; ++j)
            {
                auto& face = obj.faces[j];
                size_t vertNum = face.size();
        
                for(int j = 0; j < vertNum; ++j)
                {
                    size_t currInd = std::get<0>(face[j]);
                    size_t nextInd = std::get<0>(face[(j + 1) % vertNum]);

                    glm::vec4 currVert = mvp * obj.vertices[currInd];
                    glm::vec4 nextVert = mvp * obj.vertices[nextInd];
                    currVert /= currVert.w;
                    nextVert /= nextVert.w;


                    Brezenhem(
                        bitmap, 
                        currVert,
                        nextVert,
                        Gdiplus::Color(255, 255, 255)
                    );
                }
            }
            
        }
        );

    }

    for(auto& thread : threads)
        thread.join();
        
    graphics.DrawImage(&bitmap, 0, 0);


    //STOP DRAW
}


std::mutex g_pages_mutex;
inline void App::Brezenhem(Gdiplus::Bitmap& bitmap, glm::ivec2 start, glm::ivec2 end, const Gdiplus::Color& color)
{
    int dx = std::max(abs(end.x - start.x), 1);
    int dy = std::max(abs(end.y - start.y), 1);
    int sx = (end.x - start.x) / dx;
    int sy = (end.y - start.y) / dy;
    int err = dx - dy;

    while (start.x != end.x || start.y != end.y)
    {
        {
            std::lock_guard<std::mutex> guard(g_pages_mutex);
            bitmap.SetPixel(start.x, start.y, color);
        }

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
