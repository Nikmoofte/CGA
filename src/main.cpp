#include "config.hpp"

#include "Viewer/App/App.hpp"
int main()
{
    OUTPUT_IF_DEBUG_("Program started");
    Viewer::App app;
    return app.Run();
}
