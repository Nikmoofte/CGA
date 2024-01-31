#include "Renderer.hpp"

#include <avxintrin.h>
#include <chrono>
#include "VAO/VBLayout.hpp"


Renderer::Renderer()
{

}

void Renderer::init(size_t width, size_t height)
{
    shaderProg.createFromFile("../shaders/screen.vert", "../shaders/screen.frag");
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
    clear_screen_avx512(0x00000000);
    auto view = camera.GetViewMat();
    auto proj = camera.GetProjMat();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = camera.GetViewportMat() * proj * view * model;

    size_t facesNum = obj.faces.size();
    size_t threadFacesNum = facesNum / threads.size();
    for(int i = 0; i < threads.size(); ++i)
    {
        threads[i] = std::thread([&, i, facesNum, threadFacesNum]()
        {
            for(size_t j = i * threadFacesNum; j < (i + 1) * threadFacesNum; ++j)
            {
                auto& face = obj.faces[j];
                size_t vertNum = face.size();
        
                for(int j = 0; j < vertNum; ++j)
                {
                    size_t currInd = std::get<0>(face[j]);
                    size_t nextInd = std::get<0>(face[(j + 1) % vertNum]);

                    glm::vec4 currVert = mvp * obj.vertices[currInd];
                    glm::vec4 nextVert = mvp * obj.vertices[nextInd];
                    currVert /= currVert.w;
                    nextVert /= nextVert.w;


                    Brezenhem( 
                        currVert,
                        nextVert,
                        0xffffffff
                    );
                }
            }
            
        }
        );

    }

    for(auto& thread : threads)
        thread.join();
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

void Renderer::clear_screen_avx512(uint32_t color) 
{
    __m512i_u colorSIMD = _mm512_set1_epi32(color);
    int blockCount = static_cast<int>(width * height / 16);
    __m512i_u* blocks = (__m512i*) colorBufferMemory.data();

    //SIMD as much as possible
    for (int block = 0; block < blockCount; ++block) 
    {
        blocks[block] = colorSIMD;
    }

    //set any remaining pixels individually
    for (int pixel = blockCount * 16; pixel < width * height; ++pixel) 
    {
        colorBufferMemory[pixel] = color;
    }
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