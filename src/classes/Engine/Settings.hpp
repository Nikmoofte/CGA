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
        float fov = 90.0f;
        float ambientStrength = 0.2;
        float specularStrength = 0.5;


    private:
        Settings() = default;

    };
}


#endif