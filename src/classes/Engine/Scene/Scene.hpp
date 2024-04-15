#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <unordered_map>
#include <memory>

#include "Loader/Loader.hpp"
#include "Assets/Vertex.hpp"
#include "Assets/Light/Light.hpp"

namespace Assets
{
    class Mesh;
}

namespace Engine
{
    class Scene : public Loader::SceneBase
    {
    public:
        Scene();

        int addMesh(const std::string& filename, glm::mat4&& model);
		int addLight(const glm::vec3 &pos, const glm::vec3 &color);
        int addCamera(const glm::vec3& pos, const glm::vec3 &front, float fov, glm::ivec2 screenDimentions);

        class Camera& getCamera();
        const Camera& getCamera() const;

        std::vector<Assets::Light>& getLights() { return lights; } 
        const std::vector<Assets::Light>& getLights() const { return lights; } 

        const std::vector<Assets::Vertex>& getVertecies() const;
        const std::vector<size_t>& getIndecies() const;
        const size_t getTriangleCount() const;

        void fillBuffer();

    private:
        void Load();
        void Wait();

        std::unordered_map<std::string, int> meshMap;

        std::vector<std::unique_ptr<Assets::Mesh>> meshes;
        std::unique_ptr<Camera> camera;


        std::vector<size_t> indicies;
        std::vector<Assets::Vertex> allVertices;
        std::vector<Assets::Light> lights;
    };
}

#endif