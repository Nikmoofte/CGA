#include "config.hpp"
#include "path.hpp"
#include "Viewer/App/App.hpp"


int main(int argc, char* argv[])
{
    OUTPUT_IF_DEBUG_("Program started");

    Viewer::App app;
    return app.Run();
}
