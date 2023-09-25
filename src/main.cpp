#include <Windows.h>
#include <iostream>
#include <string>

#include "App/App.hpp"
#include "PngLoader/PngLoader.hpp"

#define DEBUG

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE,  LPSTR szCmdLine, int nCmdShow)
{
#ifdef DEBUG
    App app{"../../src/assets/smile.png"};
#else
    std::string cmd{szCmdLine};
    App app{cmd.c_str()};
#endif
    return app.run();
}
