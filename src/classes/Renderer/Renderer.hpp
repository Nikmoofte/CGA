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
#include "x86/cpu_x86.h"


class Renderer
{
public:
    Renderer();
    void init(size_t width, size_t height);
    void render(Camera& camera, Object& obj);
    void resize(size_t width, size_t height);
    void setBackFaceCulling(bool state);
    bool getBackFaceCulling(bool state);
    void toggleBackFaceCulling();
private:
    void createColorBuffer();
    void drawScreen();
    void clearScreenAVX512(uint32_t color); 
    void clearScreenAVX2(uint32_t color); 
    void clearScreen(uint32_t color); 

    void clearScreenPar(uint32_t color, size_t threadsNum); 

    inline void Brezenhem(glm::ivec2 start, glm::ivec2 end, const uint32_t color);
    inline void drawTriangle(glm::ivec2 first, glm::ivec2 second, glm::ivec2 third, const uint32_t color);

    size_t width, height;
    size_t halfWidth, halfHeight;

    bool backFaceCulling = true;

    unsigned colorBuffer;
    VAO vao;
    VBO vbo;

    std::vector<std::thread> threads{std::thread::hardware_concurrency()};
    FeatureDetector::cpu_x86 cpu;

    ShaderProg shaderProg;
    std::vector<uint32_t> colorBufferMemory;
};


#endif