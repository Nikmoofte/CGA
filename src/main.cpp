#include <iostream>
#include <string>

#define DEBUG

#include "App/App.hpp"

int main(int argc, char* argv[])
{
    if(argc <= 1)
    {
        App app{"../assets/Chariot.obj"};
        return app.run();
    }
    else
    {
        std::string cmd{argv[1]};
        App app{cmd};   
        return app.run();
    }

}
