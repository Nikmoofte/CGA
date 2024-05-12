#include "Scene.hpp"

#include "path.hpp"

#include "Assets/Mesh/Mesh.hpp"
#include "Assets/Light/Light.hpp"
#include "Assets/Material/Material.hpp"
#include "Assets/Texture/Texture.hpp"


#include "Engine/Triangle/Triangle.hpp"
#include "Engine/Camera/Camera.hpp"

#include "tiny_obj_loader.h"

#include "config.hpp"

#include "glm/glm.hpp"
#include "Assets/Texture/FlatTexture.hpp"

namespace Engine
{
    Scene::Scene()
    {
        //default texture
        addFlatTexture("default", glm::vec4(1.0f));

        //default material
        tinyobj::material_t material;
        material.name = "default";
        addMaterial(material);


        Load();
        Wait();
        fillBuffer();
    }

    Scene::~Scene()
    {
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
            meshes.push_back(std::make_unique<Assets::Mesh>(filepathString, *this));
        }
        meshInstances.emplace_back(id, std::move(model));

        return id;
    }
    int Scene::addCamera(const glm::vec3 &pos, const glm::vec3 &front, float fov, glm::ivec2 screenDimentions)
    {
        camera = std::make_unique<Camera>(pos, front, glm::radians(fov), screenDimentions);
        return 0;
    }

    int Scene::addMaterial(const tinyobj::material_t &material)
    {
        int id = -1;
        if(materialMap.contains(material.name))
        {
            id = materialMap[material.name];
        }
        else
        {
            id = materials.size();
            materialMap[material.name] = id;
            materials.emplace_back(std::make_unique<Assets::Material>(material, *this));
        }
        
        return id;
    }

    int Scene::addTexture(const std::string &filename)
    {
        int id = -1;
        if(textureMap.contains(filename))
        {
            id = textureMap[filename];
        }
        else
        {
            std::string filepathString = Path::get().fromRoot({"assets", "textures", filename}).string();
            id = textures.size();
            textureMap[filename] = id;
            textures.emplace_back(std::make_unique<Assets::Texture>(filepathString));
        }
        return id;
    }

    int Scene::addFlatTexture(const std::string &name, const glm::vec4 &val)
    {
        int id = -1;
        if(textureMap.contains(name))
        {
            id = textureMap[name];
        }
        else
        {
            id = textures.size();
            textureMap[name] = id;
            textures.emplace_back(std::make_unique<Assets::FlatTexture>(val));
        }
        return id;
    }

    std::unique_ptr<Assets::Texture> &Scene::getTexture(const std::string &filename)
    {
        return textures[textureMap[filename]];
    }   

    std::unique_ptr<Assets::Material> &Scene::getMaterial(size_t index)
    {
        return materials[index];
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

    const size_t Scene::getMaterialId(size_t index) const
    {
        for(auto& last : lastMaterialIndex)
        {
            if(index < last.first)
            {
                return last.second;
            }
        }
        return 0;
    }

    void Scene::fillBuffer()
    {
        allVertices.clear();
        indicies.clear();

        size_t offset = 0;
        size_t lastIndexOffset = 0;
        size_t id = 0;

        for(auto& meshInstance : meshInstances)
        {
            auto& mesh = meshes[meshInstance.meshId];
            auto& model = meshInstance.model;
            auto modelTransInv = glm::transpose(glm::inverse(model));
            for(auto vert : mesh->getVertices())
            {
                vert.position = model * glm::vec4(vert.position, 1.f);
				vert.normal = glm::normalize(modelTransInv * glm::vec4(vert.normal, 1.f));
                vert.tangent = glm::normalize(modelTransInv * glm::vec4(vert.tangent, 1.f));
				vert.id = id++;
				allVertices.emplace_back(std::move(vert));
            }
            for(auto& index : mesh->getIndecies())
            {
                indicies.push_back(index + offset);
            }
            //lastMaterialIndex.insert_range(lastMaterialIndex.end(), mesh->lastMaterialIndex);
            for(auto& lastIndex : mesh->lastMaterialIndex)
            {                               //last index,                       materialId
                lastMaterialIndex.push_back({lastIndex.first + lastIndexOffset, lastIndex.second});
            }
            lastIndexOffset += mesh->getIndeciesSize();
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
