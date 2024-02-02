#ifndef RENDERER_HPP
#define RENDERER_HPP

#define GLM_FORCE_SSE42

#include <glm/glm.hpp>
#include <vector>
#include <thread>

#include "ShaderProg/ShaderProg.hpp"
#include "VBO/VBO.hpp"
#include "VAO/VAO.hpp"
#include "Camera/Camera.hpp"
#include "Object/Object.hpp"

class Renderer
{
public:
    Renderer();
    void init(size_t width, size_t height);
    void render(Camera& camera, Object& obj);
    void resize(size_t width, size_t height);
private:
    void createColorBuffer();
    void drawScreen();
    void clearScreen(uint32_t color);
    void clearScreenPar(uint32_t color, size_t threadsNum); 
    inline void Brezenhem(glm::ivec2 start, glm::ivec2 end, const uint32_t color);
    inline void drawTriangle(glm::ivec2 first, glm::ivec2 second, glm::ivec2 third, const uint32_t color);

    size_t width, height;
    size_t halfWidth, halfHeight;

    unsigned colorBuffer;
    VAO vao;
    VBO vbo;

    std::vector<std::thread> threads{std::thread::hardware_concurrency()};

    ShaderProg shaderProg;
    std::vector<uint32_t> colorBufferMemory;
};


#endif