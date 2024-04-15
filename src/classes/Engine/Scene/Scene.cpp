#include "Scene.hpp"

#include "path.hpp"

#include "Assets/Mesh/Mesh.hpp"
#include "Assets/Light/Light.hpp"
#include "Engine/Triangle/Triangle.hpp"
#include "Engine/Camera/Camera.hpp"

#include "config.hpp"

#include "glm/glm.hpp"

namespace Engine
{
    Scene::Scene()
    {
        Load();
        Wait();
        fillBuffer();
    }

    int Scene::addMesh(const std::string &file, glm::mat4 &&model)
    {
        int id;
        if(meshMap.contains(file))
        {
            id = meshMap[file];
        }
        else
        {
            std::string filepathString = Path::get().fromRoot({"assets", "models", file}).string();
            OUTPUT_IF_DEBUG_(filepathString);
            id = meshes.size();
            meshMap[file] = id;
            meshes.push_back(std::make_unique<Assets::Mesh>(filepathString, std::move(model)));
        }

        return id;
    }
    int Scene::addCamera(const glm::vec3 &pos, const glm::vec3 &front, float fov, glm::ivec2 screenDimentions)
    {
        camera = std::make_unique<Camera>(pos, front, glm::radians(fov), screenDimentions);
        return 0;
    }

    Camera &Scene::getCamera()
    {
        return *camera;
    }
    const Camera &Scene::getCamera() const
    {
        return *camera;
    }

    const std::vector<Assets::Vertex> &Scene::getVertecies() const
    {
        return allVertices;
    }

    const std::vector<size_t> &Scene::getIndecies() const
    {
        return indicies;
    }

    const size_t Scene::getTriangleCount() const
    {
        return indicies.size() / 3;
    }

    void Scene::fillBuffer()
    {
        allVertices.clear();
        indicies.clear();

        size_t offset = 0;
        size_t id = 0;

        for(auto& mesh : meshes)
        {
            auto& model = mesh->getModelMat();
            auto modelTransInv = glm::transpose(glm::inverse(model));
            for(auto& vert : mesh->getVertices())
            {
                vert.position = model * glm::vec4(vert.position, 1.f);
				vert.normal = glm::normalize(modelTransInv * glm::vec4(vert.normal, 1.f));
				vert.id = id++;
				allVertices.push_back(vert);
            }
            for(auto& index : mesh->getIndecies())
            {
                indicies.push_back(index + offset);
            }
            offset += mesh->getVerticesSize();
        }
    }

    void Scene::Load()
    {
        if(!Loader::LoadSceneFromFile(Path::get().fromRoot({"assets", "BaseScene.scene"}), *this))
        {
            OUTPUT_IF_DEBUG_("Failed to load scene");
        }
    }

    void Scene::Wait()
    {
        for(auto& mesh : meshes)
            mesh->Wait();
    }

    int Scene::addLight(const glm::vec3 &pos, const glm::vec3 &color)
    {
        int id = lights.size();
        lights.emplace_back(pos, color);
        return id;
    }
}
