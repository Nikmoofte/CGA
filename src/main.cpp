#include <Windows.h>
#include <iostream>
#include <string>

#define DEBUG

#include "App/App.hpp"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE,  LPSTR szCmdLine, int nCmdShow)
{
#ifdef DEBUG
    App app{"../src/assets/Cube.obj"};
#else
    std::string cmd{szCmdLine};
    App app{cmd.c_str()};
#endif
    return app.run();
}
