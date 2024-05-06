#include "Loader.hpp"

#include <fstream>
#include <sstream>
#include <iterator>

#include <glm/gtc/matrix_transform.hpp>
#include <Engine/Settings.hpp>
#include "config.hpp"

namespace Loader
{
    constexpr size_t MAX_LINE_SIZE = 1024;
    /*
        Scene consist of
        Light pos
        Model/Mesh
        Camera
    
    */
    bool LoadSceneFromFile(const std::filesystem::path& filepath, SceneBase& scene)
    {
        OUTPUT_IF_DEBUG_(filepath);
        std::ifstream file(filepath);
        bool result = file.is_open(); 
        if (result)
        {
            auto& settings = Engine::Settings::Get();
            std::string line;
            while(std::getline(file, line))
            {
                //Mesh parce
                if(line.contains("mesh"))
                {
                    std::string path;
                    glm::mat4 model{1.0f};
                    while(std::getline(file, line))
                    {
                        if(line.contains("}"))
                            break;
                        size_t pos = 0;
                        if((pos = line.find("file")) != std::string::npos)
                        {
                            path = line.substr(pos + 4 + 1);
                            continue;
                        }
                        if((pos = line.find("pos")) != std::string::npos)
                        {
                            std::stringstream ss(line.substr(pos + 3 + 1));
                            glm::vec3 pos{0.0f};
                            ss >> pos.x;
                            ss >> pos.y;
                            ss >> pos.z;
                            model = glm::translate(model, pos);
                            continue;
                        }
                        if((pos = line.find("scale")) != std::string::npos)
                        {
                            std::stringstream ss(line.substr(pos + 5 + 1));
                            glm::vec3 scale{1.0f};
                            ss >> scale.x >> scale.y >> scale.z;
                            model = glm::scale(model, scale);
                            continue;
                        }
                    }
                    if(!path.empty())
                    {
                        scene.addMesh(path, std::move(model));
                    }
                }

                //Light parce
                if(line.contains("light"))
                {
                    glm::vec3 lightPos{0.0f};
                    glm::vec3 color{1.0f};
                    while(std::getline(file, line))
                    {
                        if(line.contains("}"))
                            break;
                        size_t pos = 0;
                        if((pos = line.find("pos")) != std::string::npos)
                        {
                            std::stringstream ss(line.substr(pos + 3 + 1));
                            ss >> lightPos.x >> lightPos.y >> lightPos.z;
                            continue;
                        }
                        if((pos = line.find("color")) != std::string::npos)
                        {
                            std::stringstream ss(line.substr(pos + 5 + 1));
                            ss >> color.x >> color.y >> color.z;
                            continue;
                        }
                    }
                    scene.addLight(lightPos, color);
                }

                //Camera parce
                if(line.contains("camera"))
                {
                    glm::vec3 cameraPos{0.0f};
                    glm::vec3 cameraFront{1.0f, 0.0f, 0.0f};
                    float fov;
                    while(std::getline(file, line))
                    {
                        if(line.contains("}"))
                            break;
                        size_t pos = 0;
                        if((pos = line.find("pos")) != std::string::npos)
                        {
                            std::stringstream ss(line.substr(pos + 3 + 1));
                            ss >> cameraPos.x >> cameraPos.y >> cameraPos.z;
                            continue;
                        }
                        if((pos = line.find("front")) != std::string::npos)
                        {
                            std::stringstream ss(line.substr(pos + 5 + 1));
                            ss >> cameraFront.x >> cameraFront.y >> cameraFront.z;
                            glm::normalize(cameraFront);
                            continue;
                        }
                        if((pos = line.find("fov")) != std::string::npos)
                        {
                            std::stringstream ss(line.substr(pos + 3 + 1));
                            ss >> fov;
                            glm::normalize(cameraFront);
                            continue;
                        }
                    }
                    scene.addCamera(cameraPos, cameraFront, fov, settings.windowSize);
                }
            }
        }
        return result;
    }

}