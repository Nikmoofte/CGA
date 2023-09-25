#include <Windows.h>
#include <iostream>
#include <string>

#define DEBUG

#include "App/App.hpp"
#include "PngLoader/PngLoader.hpp"


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE,  LPSTR szCmdLine, int nCmdShow)
{
#ifdef DEBUG
    App app{"../../src/assets/dog.png"};
#else
    std::string cmd{szCmdLine};
    App app{cmd.c_str()};
#endif
    return app.run();
}
