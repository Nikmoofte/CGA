#ifndef RENDERER_HPP
#define RENDERER_HPP

#define GLM_FORCE_SSE42

#include <glm/glm.hpp>
#include <vector>
#include <list>
#include <thread>

#include "ShaderProg/ShaderProg.hpp"
#include "VBO/VBO.hpp"
#include "VAO/VAO.hpp"
#include "Camera/Camera.hpp"
#include "Object/Object.hpp"
#include "x86/cpu_x86.h"
#include "BS_thread_pool.hpp"


class Renderer
{
public:
    Renderer(BS::thread_pool& pool) : threads(pool){ cpu.detect_host(); };
    void init(size_t width, size_t height);
    void render(Camera& camera, Object& obj);
    void resize(size_t width, size_t height, const glm::mat4& viewportProj);

    void setBackFaceCulling(bool state);
    bool getBackFaceCulling(bool state);
    void toggleBackFaceCulling();
    void toggleRasterisation();
    void toggleWireframe();
    void toggleShowDepth();
private:
    void createColorBuffer();
    void drawScreen();
    void clearScreenAVX512(uint32_t color); 
    void clearScreenAVX2(uint32_t color); 
    void clearScreen(uint32_t color); 
    uint32_t crunchColor(glm::vec3& color);

    inline void Brezenhem(glm::vec3 start, glm::vec3 end, const uint32_t color);
    inline std::list<int> BrezenhemGetList(glm::ivec2 start, glm::ivec2 end);
    inline void verticalLine(int x, int y1, int y2, float z, const uint32_t color);
    inline void drawTriangle(glm::ivec2 first, glm::ivec2 second, glm::ivec2 third, float z, const uint32_t color);

    inline void setPixel(glm::vec3 pos, uint32_t color);
    inline void setPixel(int index, float z, uint32_t color);
    inline float getPixelDepth(glm::ivec2 pos);

    glm::mat4 vProj;

    size_t width, height;
    size_t halfWidth, halfHeight;

    bool backFaceCulling = false;
    bool rasterisation = false;
    bool wireframe = true;
    bool showDepth = false;

    unsigned colorBuffer;
    VAO vao;
    VBO vbo;

    //std::vector<std::thread> threads{std::thread::hardware_concurrency()};
    FeatureDetector::cpu_x86 cpu;
    BS::thread_pool& threads;

    ShaderProg shaderProg;
    std::vector<uint32_t> colorBufferMemory;
    std::vector<float> zBufferMemory;
};


#endif