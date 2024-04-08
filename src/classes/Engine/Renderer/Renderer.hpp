#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <memory>

#include <Assets/Vertex.hpp>

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
        void render();
    private:
        void vertexShaderStage();

        std::unique_ptr<Buffers::ColorBuffer> colorBuffer; 
        std::unique_ptr<Buffers::DepthBuffer> depthBuffer; 

        std::vector<Assets::Vertex> projectedVertices;
        const Scene& scene;
    };
}

#endif