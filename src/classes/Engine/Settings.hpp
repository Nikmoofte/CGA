#ifndef SETTINGS_HPP    
#define SETTINGS_HPP

#include <glm/glm.hpp>

//Singletone class
namespace Engine
{
    struct Settings
    {
        static Settings& Get()
        {
            static Settings instance;
            return instance;
        }

        glm::ivec2 windowSize{800, 600};
        float resolutionScale = 1.0f;

    private:
        Settings() = default;

    };
}


#endif