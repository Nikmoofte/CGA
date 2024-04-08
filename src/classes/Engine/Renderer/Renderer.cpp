#include "Renderer.hpp"

#include "Engine/Scene/Scene.hpp"
#include "Engine/Buffers/ColorBuffer/ColorBuffer.hpp"
#include "Engine/Buffers/DepthBuffer/DepthBuffer.hpp"
#include "Engine/Camera/Camera.hpp"

namespace Engine
{
    Renderer::Renderer(const Scene &scene) : 
    scene(scene)
    {
        resetBuffers();
    }

    void Renderer::clearBuffers()
    {
        colorBuffer->clear();
        depthBuffer->clear(1.0f);
    }
    void Renderer::resetBuffers()
    {
        size_t pixelCount = scene.getCamera().getPixelCount();
        colorBuffer = std::make_unique<Buffers::ColorBuffer>(pixelCount);
        depthBuffer = std::make_unique<Buffers::DepthBuffer>(pixelCount);
        clearBuffers();
    }
    void Renderer::render()
    {
        clearBuffers();

        vertexShaderStage();
    }
    void Renderer::vertexShaderStage()
    {
        
    }
}