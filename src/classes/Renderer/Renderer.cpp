#include "Renderer.hpp"

#include <avxintrin.h>
#include <iostream>
#include <chrono>
#include <execution>
#include <algorithm>
#include "VAO/VBLayout.hpp"

#include "config.hpp"
#include "FrustumCull.h"

#define SINGLEFUNCTION

Renderer::Renderer()
{
    cpu.detect_host();
}

void Renderer::init(size_t width, size_t height)
{
    OUTPUT_IF_DEBUG_("Renderer initialisation start")
    std::string vert = 
    "#version 460 core\n"
    "layout (location = 0) in vec2 vertexPosition;\n"
    "layout (location = 1) in vec2 vertexTexCoords;\n"
    "out vec2 fragmentTexCoords;\n"
    "void main()\n"
    "{\n"
        "gl_Position = vec4(vertexPosition, 0.0, 1.0);\n"
        "fragmentTexCoords = vertexTexCoords;\n"
    "}\n";
    
    std::string frag = 
    "#version 460 core\n"
    "in vec2 fragmentTexCoords;\n"
    "uniform sampler2D frameBuffer;\n"
    "out vec4 finalColor;\n"
    "void main()\n"
    "{\n"
        "finalColor = texture(frameBuffer, fragmentTexCoords);\n"
    "}\n";
    shaderProg.create(vert, frag);
    shaderProg.Use();

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

    this->width = width;
    this->height = height;
    halfWidth = width / 2;
    halfHeight = height / 2;
    
    float vertex[] =
    {
			 1.0f,  1.0f, 0.0f, 1.0f, // top right
			-1.0f,  1.0f, 0.0f, 0.0f, //top left
		    -1.0f, -1.0f, 1.0f, 0.0f, //bottom left

			-1.0f, -1.0f, 1.0f, 0.0f, //bottom left
			 1.0f, -1.0f, 1.0f, 1.0f, //bottom right
			 1.0f,  1.0f, 0.0f, 1.0f, // top right
	};
    vbo.SetBufferData(sizeof(vertex), vertex, GL_STATIC_DRAW);
    VBLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    vao.setLayout(vbo, layout);

    createColorBuffer();
}

void Renderer::createColorBuffer() 
{

    unsigned int tempWidth = width;
    unsigned int tempHeight = height;

    glGenTextures(1, &colorBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glTextureParameteri(colorBuffer, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(colorBuffer, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(colorBuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(colorBuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    colorBufferMemory.resize(width * height);
    zBufferMemory.resize(width * height);

}

void Renderer::resize(size_t width, size_t height)
{
    this->width = width;
    this->height = height;
    halfWidth = width / 2;
    halfHeight = height / 2;
    colorBufferMemory.resize(width * height);
    zBufferMemory.resize(width * height);
}

uint32_t Renderer::crunchColor(const glm::vec3& color) 
{

    uint8_t r = std::max(0, std::min(255, (int)(255 * color.x)));
    uint8_t g = std::max(0, std::min(255, (int)(255 * color.y)));
    uint8_t b = std::max(0, std::min(255, (int)(255 * color.z)));
    return (r) + (g << 8) + (b << 16);
}

void Renderer::render(Camera& camera, Object& obj)
{
    constexpr uint32_t clearColor = 0x00505050;
    const glm::vec3 defColor = {1.0f, 1.0f, 1.0f};
    constexpr uint32_t wireframeColor = 0;

    static bool first = true;
    if(first)
    {
        OUTPUT_IF_DEBUG_("clearing begin");
        first = false;
    }
    if(cpu.HW_AVX512_CD & cpu.HW_AVX512_BW & cpu.HW_AVX512_DQ)
        clearScreenAVX512(clearColor);
    else 
    if(cpu.HW_AVX2)
        clearScreenAVX2(clearColor);
    else
        clearScreen(clearColor);
    
    auto view = camera.GetViewMat();
    auto proj = camera.GetProjMat();
    glm::mat4 mvp = proj * view;
    size_t facesNum = obj.faces.size();
    size_t threadsNum = threads.size() < facesNum ? threads.size() : facesNum;   
    size_t threadFacesNum = facesNum / threadsNum;

    glm::vec4 lightPos(5.0f, 0.0f, 0.0f, 1.0f);

    for(int i = 0; i < threadsNum; ++i)
    {
        threads[i] = std::thread([&, i, facesNum, threadFacesNum]()
        {
            for(size_t j = i * threadFacesNum; j < (i + 1) * threadFacesNum; ++j)
            {
                auto& face = obj.faces[j];
                size_t vertNum = face.size();
        
                size_t firstInd = std::get<0>(face[0]);
                size_t secondInd = std::get<0>(face[1]);
                size_t thirdInd = std::get<0>(face[2]);

                glm::vec4 firstVert = obj.vertices[firstInd];
                glm::vec4 secondVert = obj.vertices[secondInd];
                glm::vec4 thirdVert = obj.vertices[thirdInd];
                glm::vec4 center = (firstVert + secondVert + thirdVert) / 3.0f;
                glm::vec4 light = lightPos;

                firstVert = mvp * firstVert;
                secondVert = mvp * secondVert;
                thirdVert = mvp * thirdVert;

                firstVert /= firstVert.w;
                secondVert /= secondVert.w;
                thirdVert /= thirdVert.w;
                //light /= light.w;
                
                glm::vec4 revLightVec = glm::normalize(light - center);

                glm::vec3 tangent = secondVert - firstVert;
                glm::vec3 bitangent = thirdVert - firstVert;
                glm::vec4 normal = glm::normalize(glm::vec4(glm::cross(tangent, bitangent), 0.0f));
                
                auto color = defColor * glm::dot(normal, revLightVec); 
                uint32_t colorUI = crunchColor(color);

                if(backFaceCulling && glm::dot(normal, (mvp * glm::vec4(camera.GetPos(), 1.0f) - firstVert) ) > 0)
                        continue;              


                firstVert = camera.GetViewportMat() * firstVert;
                secondVert = camera.GetViewportMat() * secondVert;
                thirdVert = camera.GetViewportMat() * thirdVert;

                if(rasterisation)
                {
                    drawTriangle(firstVert, secondVert, thirdVert, (firstVert.z + secondVert.z + thirdVert.z) / 3.0f, colorUI);
                }
                if(wireframe)
                {
                    Brezenhem(firstVert, secondVert, wireframeColor);
                    Brezenhem(secondVert, thirdVert, wireframeColor);
                    Brezenhem(thirdVert, firstVert, wireframeColor);
                }
            }
            
        }
        );

    }

    for(int i = 0; i < threadsNum; ++i)
        threads[i].join();

    drawScreen();
}

inline void Renderer::Brezenhem(glm::vec3 start, glm::vec3 end, const uint32_t color)
{
    start = glm::floor(start);
    end = glm::floor(end);

    if(start.x < 0.0f || start.x > width - 1 || start.y < 0.0f || start.y > height - 1)
        return;
    if(end.x < 0.0f || end.x > width - 1 || end.y < 0.0f || end.y > height - 1)
        return;

    // start.x = std::clamp(start.x, 0.0f, (float)width - 1);
    // start.y = std::clamp(start.y, 0.0f, (float)height - 1);
    // end.x = std::clamp(end.x, 0.0f, (float)width - 1);
    // end.y = std::clamp(end.y, 0.0f, (float)height - 1);


    int dx = std::max(abs(end.x - start.x), 1.0f);
    int dy = std::max(abs(end.y - start.y), 1.0f);
    int sx = (end.x - start.x) / dx;
    int sy = (end.y - start.y) / dy;
    int err = dx - dy;
    
    while (start.x != end.x || start.y != end.y)
    {
        //std::lock_guard<std::mutex> guard(g_pages_mutex);
        setPixel(start, color);


        int e2 = err << 1;
        if (e2 > -dy)
        {
            err -= dy;
            start.x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            start.y += sy;
        }
    }
}

inline std::list<int> Renderer::BrezenhemGetList(glm::ivec2 start, glm::ivec2 end)
{
    std::list<int> yCoordianates;
    int dx = std::max(abs(end.x - start.x), 1);
    int dy = std::max(abs(end.y - start.y), 1);
    int sx = (end.x - start.x) / dx;
    int sy = (end.y - start.y) / dy;
    int err = dx - dy;
    

    int prevX = -1;
    while (start.x != end.x || start.y != end.y)
    {
        if(prevX != start.x)
            yCoordianates.push_back(start.y);
        prevX = start.x;

        int e2 = err << 1;
        if (e2 > -dy)
        {
            err -= dy;
            start.x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            start.y += sy;
        }
    }
    return yCoordianates;
}

inline void Renderer::verticalLine(int x, int y1, int y2, float z, const uint32_t color)
{
    if(x >= width || x < 0)
        return;
    if(y1 >= height || y1 < 0)
        return;
    if(y2 >= height || y2 < 0)
        return;

    int dy = y2 - y1; 
    int startIndex = x * height + y1;
    for (int pixel = startIndex; pixel <= startIndex + dy; ++pixel) 
    {
        setPixel(pixel, z, color);
    }
}

inline void Renderer::drawTriangle(glm::ivec2 first, glm::ivec2 second, glm::ivec2 third, float z, const uint32_t color)
{
    if(first.x < 0 || first.x >= width || first.y < 0 || first.y >= height)
        return;
    if(second.x < 0 || second.x >= width || second.y < 0 || second.y >= height)
        return;
    if(third.x < 0 || third.x >= width || third.y < 0 || third.y >= height)
        return;

    if (second.x < first.x) { std::swap(second, first); }
    if (third.x < first.x) { std::swap(third, first); }
    if (third.x < second.x) { std::swap(third, second); }

    auto y01 = BrezenhemGetList(first, second);
    auto y12 = BrezenhemGetList(second, third);
    auto y02 = BrezenhemGetList(first, third);

    y01.append_range(y12);
    y02.push_back(third.y);
    if(y01.empty() || y02.empty())
        return;

    auto y02it = y02.begin();
    auto y01it = y01.begin();
    std::advance(y02it, y02.size() >> 1);
    std::advance(y01it, y01.size() >> 1);
    std::list<int>* yLeft, *yRight;
    if (*y02it < *y01it) 
    {
        yLeft = &y02;
        yRight = &y01;
    } 
    else 
    {
        yLeft = &y01;
        yRight = &y02;
    }

    int i = first.x;
    for(auto left = yLeft->begin(), right = yRight->begin(); left != yLeft->end() && right != yRight->end(); ++left, ++right)
    {
        verticalLine(i, *left, *right, z, color);
        ++i;
    }

}

void Renderer::clearScreenAVX512(uint32_t color) 
{
    constexpr unsigned blockSize = 16;
    constexpr float defaulDepth = 1.0f;
    
    __m512i_u colorSIMD = _mm512_set1_epi32(color);
    __m512 zSIMD = _mm512_set1_ps(defaulDepth);
    int blockCount = static_cast<int>(width * height / blockSize);
    uint32_t* blocks = colorBufferMemory.data();
    float* zBlocks = zBufferMemory.data();

    for (int block = 0; block < width * height; block += blockSize) 
    {
        //blocks[block] = colorSIMD;
        _mm512_storeu_epi32(blocks + block, colorSIMD);
        _mm512_storeu_ps(zBlocks + block, zSIMD);
    }

    for (int pixel = blockCount * blockSize; pixel < width * height; ++pixel) 
    {
        colorBufferMemory[pixel] = color;
        zBufferMemory[pixel] = defaulDepth;
    }

}
void Renderer::clearScreenAVX2(uint32_t color) 
{
    constexpr unsigned blockSize = 8;
    constexpr float defaulDepth = 1.0f;

    __m256i_u colorSIMD = _mm256_set1_epi32(color);
    __m256 zSIMD = _mm256_set1_ps(defaulDepth);
    int blockCount = static_cast<int>(width * height / blockSize);
    __m256i_u* blocks = (__m256i_u*) colorBufferMemory.data();
    __m256* zBlocks = (__m256*) zBufferMemory.data();

    for (int block = 0; block < blockCount; ++block) 
    {
        blocks[block] = colorSIMD;
        zBlocks[block] = zSIMD;
    }

    for (int pixel = blockCount * blockSize; pixel < width * height; ++pixel) 
    {
        colorBufferMemory[pixel] = color;
        zBufferMemory[pixel] = defaulDepth;
    }

}

void Renderer::clearScreen(uint32_t color)
{
    constexpr float defaulDepth = 0.0f;
    for (int pixel = 0; pixel < width * height; ++pixel) 
    {
        colorBufferMemory[pixel] = color;
        zBufferMemory[pixel] = defaulDepth;
    }
}

inline void Renderer::setPixel(glm::vec3 pos, uint32_t color)
{
    int index = (int)pos.x * height + (int)pos.y;
    if(index > colorBufferMemory.size() || index < 0)
        return;
    setPixel(index, pos.z, color);
}
inline void Renderer::setPixel(int index, float z, uint32_t color)
{
    if(zBufferMemory[index] > z)
    {
        colorBufferMemory[index] = color;
        zBufferMemory[index] = z;
    }
}

inline float Renderer::getPixelDepth(glm::ivec2 pos)
{
    if(pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height)
        return 1000.0f;
    return zBufferMemory[pos.x * height + pos.y];
}

void Renderer::drawScreen()
{
    shaderProg.Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    if(showDepth)
    {
        //std::ranges::for_each(zBufferMemory, [](auto& elem) { elem = 1.0f - elem + 0.2f; });
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, height, width, 0,  GL_DEPTH_COMPONENT, GL_FLOAT, zBufferMemory.data());
    }
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, height, width, 0,  GL_RGBA, GL_UNSIGNED_BYTE, colorBufferMemory.data());

    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glFlush();

}

void Renderer::setBackFaceCulling(bool state)
{
    backFaceCulling = state;
}

bool Renderer::getBackFaceCulling(bool state)
{
    return backFaceCulling;
}

void Renderer::toggleBackFaceCulling()
{
    backFaceCulling = !backFaceCulling;
}

void Renderer::toggleRasterisation()
{
    rasterisation = !rasterisation;
}

void Renderer::toggleWireframe()
{
    wireframe = !wireframe;
}

void Renderer::toggleShowDepth()
{
    showDepth = !showDepth;
}
