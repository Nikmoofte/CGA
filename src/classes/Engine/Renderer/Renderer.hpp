#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <memory>
#include <thread>

#include <Assets/Vertex.hpp>
#include <Engine/Triangle/Triangle.hpp>

#include <glm/vec3.hpp>

namespace Engine
{
    namespace Buffers
    {
        class ColorBuffer;
        class DepthBuffer;
    }

    class Renderer
    {
    public:
        Renderer(const class Scene& scene);

        void clearBuffers();
        void resetBuffers();
        void* getColorBuffer();
        void render();
    private:
        void vertexShaderStage();
        void clippingStage();
        void rasterisationStage();
        void fragmentShaderStage();
        void triangleAssing();
        glm::vec3 getBarycentric(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

        std::unique_ptr<Buffers::ColorBuffer> colorBuffer; 
        std::unique_ptr<Buffers::DepthBuffer> depthBuffer; 

        std::vector<Triangle> triangles;
        size_t coreInterval;
        std::vector<int> coresID;

        std::vector<Assets::Vertex> projectedVertices;
        const Scene& scene;
    };


}

#endif