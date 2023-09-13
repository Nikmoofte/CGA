#include <Windows.h>



int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE,  PWSTR szCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";

    WNDCLASSW wc = { };

    wc.lpfnWndProc   = DefWindowProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = CLASS_NAME;
    
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
    0,                              // Optional window styles.
    CLASS_NAME,                     // Window class
    L"Learn to Program Windows",    // Window text
    WS_OVERLAPPEDWINDOW,            // Window style

    // Size and position
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

    NULL,       // Parent window    
    NULL,       // Menu
    hInst,  // Instance handle
    NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, nullptr, NULL, NULL))
    {
        DispatchMessage(&msg);
    }


    return 0;
}