#include <iostream>
#include <string>

#include "App/App.hpp"
#include "config.hpp"

int main(int argc, char* argv[])
{
    OUTPUT_IF_DEBUG_("main start")

    try
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
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    


}
