#include "Renderer.hpp"

#include <avxintrin.h>
#include <chrono>
#include <execution>
#include <algorithm>
#include "VAO/VBLayout.hpp"

#define NOSINGLEFUNCTION

Renderer::Renderer()
{

}

void Renderer::init(size_t width, size_t height)
{
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

void Renderer::render(Camera& camera, Object& obj)
{
    clearScreen(0x00000000);
    
    auto view = camera.GetViewMat();
    auto proj = camera.GetProjMat();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = camera.GetViewportMat() * proj * view * model;

    size_t facesNum = obj.faces.size();
    size_t threadsNum = threads.size() < facesNum ? threads.size() : facesNum;   
    size_t threadFacesNum = facesNum / threadsNum;
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

                glm::vec4 firstVert = mvp * obj.vertices[firstInd];
                glm::vec4 secondVert = mvp * obj.vertices[secondInd];
                glm::vec4 thirdVert = mvp * obj.vertices[thirdInd];
                firstVert /= firstVert.w;
                secondVert /= secondVert.w;
                thirdVert /= thirdVert.w;

#ifdef SINGLEFUNCTION
                drawTriangle(firstVert, secondVert, thirdVert, 0xffffffff);
#else
                uint32_t color = 0xffffffff;
                Brezenhem(firstVert, secondVert, color);
                Brezenhem(secondVert, thirdVert, color);
                Brezenhem(thirdVert, firstVert, color);
#endif
            }
            
        }
        );

    }

    for(int i = 0; i < threadsNum; ++i)
        threads[i].join();
    drawScreen();
}

void Renderer::resize(size_t width, size_t height)
{
    this->width = width;
    this->height = height;
    halfWidth = width / 2;
    halfHeight = height / 2;
    colorBufferMemory.resize(width * height);
}

void Renderer::clearScreen(uint32_t color) 
{
    __m512i_u colorSIMD = _mm512_set1_epi32(color);
    int blockCount = static_cast<int>(width * height / 16);
    __m512i_u* blocks = (__m512i*) colorBufferMemory.data();

    for (int block = 0; block < blockCount; ++block) 
    {
        blocks[block] = colorSIMD;
    }

    for (int pixel = blockCount * 16; pixel < width * height; ++pixel) 
    {
        colorBufferMemory[pixel] = color;
    }

}

//TOO SLOW
void Renderer::clearScreenPar(uint32_t color, size_t threadsNum)
{
    int blockPerThread = static_cast<int>((width * height >> 4) / threadsNum);
    for(int i = 0; i < threadsNum; ++i)
    {
        threads[i] = std::thread([&, i, blockPerThread, color]()
        {
            __m512i_u colorSIMD = _mm512_set1_epi32(color);
            int blockCount = static_cast<int>(width * height / 16);
            __m512i_u* blocks = (__m512i*) colorBufferMemory.data();

            for (int block = i * blockPerThread; block < (i + 1) * blockPerThread; ++block) 
            {
                blocks[block] = colorSIMD;
            }

            for (int pixel = i * blockPerThread * 16; pixel < (i + 1) * blockPerThread; ++pixel) 
            {
                colorBufferMemory[pixel] = color;
            }
        }
        );
    }

    for(int i = 0; i < threadsNum; ++i)
        threads[i].join();
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
    glTextureParameteri(colorBuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(colorBuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    colorBufferMemory.resize(width * height);

}

inline void Renderer::Brezenhem(glm::ivec2 start, glm::ivec2 end, const uint32_t color)
{
    start.x = std::max(0, std::min((int)width - 1, start.x));
    start.y = std::max(0, std::min((int)height - 1, start.y));
    end.x = std::max(0, std::min((int)width - 1, end.x));
    end.y = std::max(0, std::min((int)height - 1, end.y));


    int dx = std::max(abs(end.x - start.x), 1);
    int dy = std::max(abs(end.y - start.y), 1);
    int sx = (end.x - start.x) / dx;
    int sy = (end.y - start.y) / dy;
    int err = dx - dy;
    
    while (start.x != end.x || start.y != end.y)
    {
        //std::lock_guard<std::mutex> guard(g_pages_mutex);
        colorBufferMemory[start.x * height + start.y] = color;


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

inline void Renderer::drawTriangle(glm::ivec2 first, glm::ivec2 second, glm::ivec2 third, const uint32_t color)
{
    Brezenhem(first, second, color);
    Brezenhem(second, third, color);
    Brezenhem(third, first, color);
}

void Renderer::drawScreen()
{
    shaderProg.Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, height, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorBufferMemory.data());

    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glFlush();

}