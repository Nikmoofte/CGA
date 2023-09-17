#include <Windows.h>
#include <iostream>

#include "App/App.hpp"


int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE,  PWSTR szCmdLine, int nCmdShow)
{
    App app{};
    return app.run();
}
