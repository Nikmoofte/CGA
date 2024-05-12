#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <unordered_map>
#include <memory>

#include "Loader/Loader.hpp"
#include "Assets/Vertex.hpp"
#include "Assets/Light/Light.hpp"
#include "Assets/MeshInstance.hpp"

#include "tiny_obj_loader.h"

namespace Assets
{
    class Mesh;
    class Material;
    class Texture;
}

namespace Engine
{
    class Scene : public Loader::SceneBase
    {
    public:
        Scene();
        ~Scene();
        int addMesh(const std::string& filename, glm::mat4&& model);
		int addLight(const glm::vec3 &pos, const glm::vec3 &color);
        int addCamera(const glm::vec3& pos, const glm::vec3 &front, float fov, glm::ivec2 screenDimentions);
        int addMaterial(const tinyobj::material_t& material);
        int addTexture(const std::string& filename);
        int addFlatTexture(const std::string& name, const glm::vec4& val);
        std::unique_ptr<Assets::Texture>& getTexture(const std::string& filename);
        std::unique_ptr<Assets::Material>& getMaterial(size_t index);

        class Camera& getCamera();
        const Camera& getCamera() const;

        std::vector<Assets::Light>& getLights() { return lights; } 
        const std::vector<Assets::Light>& getLights() const { return lights; } 


        const std::vector<Assets::Vertex>& getVertecies() const;
        const std::vector<size_t>& getIndecies() const;
        const size_t getTriangleCount() const;
        const size_t getMaterialId(size_t index) const;

        void fillBuffer();

    private:
        void Load();
        void Wait();

        std::unique_ptr<Camera> camera;
        
        std::unordered_map<std::string, int> meshMap;
        std::unordered_map<std::string, int> materialMap;
        std::unordered_map<std::string, int> textureMap;

        std::vector<Assets::MeshInstance> meshInstances;
        std::vector<std::unique_ptr<Assets::Mesh>> meshes;

        std::vector<Assets::Vertex> allVertices;
        std::vector<size_t> indicies;
		std::vector<std::pair<size_t, size_t>> lastMaterialIndex;

        std::vector<Assets::Light> lights;

        std::vector<std::unique_ptr<Assets::Material>> materials;   
        std::vector<std::unique_ptr<Assets::Texture>> textures;
    };
}

#endif