#include "App.hpp"

#include <stdexcept>
#include <memory>
#include <chrono>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ObjParser/ObjParser.hpp"

bool resize = true;

App::App(size_t width, size_t height)
{
    appWidth = width;
    appHeight = height;

    window = makeWindow();
    renderer = std::make_unique<Renderer>();
    renderer->init(appWidth, appHeight);
}

App::App(const std::string& filePath) : App(800, 600)
{
    obj = ObjParser{}(filePath);
}

App::~App()
{
    glfwTerminate();
}

GLFWwindow* App::makeWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(appWidth, appHeight, "This is working I hope", NULL, NULL);

	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glViewport(0, 0, appWidth, appHeight);

    glfwSetWindowSizeCallback(window, [](GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
        ::resize = true;
    }
    );

	return window;
}



void App::calculateFPS()
{
    static int i = 0;
    if(i++ < 100)
        return;
    delta = std::chrono::system_clock::now() - frameStart;
    std::stringstream title;
    title << "Running at " << 1 / delta.count() << " fps.";
    glfwSetWindowTitle(window, title.str().c_str());
    i = 0;
}

void App::resize()
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    width = std::max(width, 1);
    height = std::max(height, 1);
    appWidth = width;
    appHeight = height;
    renderer->resize(appWidth, appHeight);
    camera.ChangeView(appWidth, appHeight);
    ::resize = false;
}

void App::processInput(GLFWwindow* window) 
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
    {
		glfwSetWindowShouldClose(window, true);
        return;
	}
    camera.KeyboardControl(window, delta.count());
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        camera.MouseControl(window);

	return;
}

int App::run()
{
    while (!glfwWindowShouldClose(window))
    {
        frameStart = std::chrono::system_clock::now();  
        processInput(window);
        glfwPollEvents();
        if(::resize)
            resize();


		//update logic can be handled here

        camera.RecalcView();
		//draw (no arguments right now, as no real data exists for drawing)
		renderer->render(camera, obj);
        
        calculateFPS();
    }
    return 0;
}

// inline void App::draw()
// {        
//     RECT screenSpace{0, 0, (LONG)appWidht, (LONG)appHeight};

//     auto memDC = CreateCompatibleDC(dc);
//     auto memBM = CreateCompatibleBitmap(dc, appWidht, appHeight);
//     auto prev = SelectObject(memDC, memBM);

    
//     BITMAPINFO bitmapInfo{};
//     bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//     bitmapInfo.bmiHeader.biWidth = appWidht;
//     bitmapInfo.bmiHeader.biHeight = -appHeight;
//     bitmapInfo.bmiHeader.biPlanes = 1;
//     bitmapInfo.bmiHeader.biBitCount = 32;
//     bitmapInfo.bmiHeader.biCompression = BI_RGB;

//     std::vector<DWORD32> bitmapData;
//     bitmapData.resize(appWidht * appHeight);
//     //DRAW
//     std::chrono::duration<double> time = std::chrono::system_clock::now() - appStart;
//     auto view = camera.GetViewMat();
//     auto proj = camera.GetProjMat();
//     glm::mat4 model = glm::mat4(1.0f);
//     glm::mat4 mvp = camera.GetViewportMat() * proj * view * model;

//     size_t facesNum = obj.faces.size();
//     size_t threadFacesNum = facesNum / threads.size();
//     for(int i = 0; i < threads.size(); ++i)
//     {
//         threads[i] = std::thread([&, i, facesNum, threadFacesNum]()
//         {
//             for(size_t j = i * threadFacesNum; j < (i + 1) * threadFacesNum; ++j)
//             {
//                 auto& face = obj.faces[j];
//                 size_t vertNum = face.size();
        
//                 for(int j = 0; j < vertNum; ++j)
//                 {
//                     size_t currInd = std::get<0>(face[j]);
//                     size_t nextInd = std::get<0>(face[(j + 1) % vertNum]);

//                     glm::vec4 currVert = mvp * obj.vertices[currInd];
//                     glm::vec4 nextVert = mvp * obj.vertices[nextInd];
//                     currVert /= currVert.w;
//                     nextVert /= nextVert.w;


//                     Brezenhem(
//                         bitmapData, 
//                         currVert,
//                         nextVert,
//                         Gdiplus::Color(255, 255, 255)
//                     );
//                 }
//             }
            
//         }
//         );

//     }

//     for(auto& thread : threads)
//         thread.join();
    
//     SetDIBits(memDC, memBM, 0, appHeight, &bitmapData.front(), &bitmapInfo, DIB_RGB_COLORS);
//     BitBlt(dc, 0, 0, appWidht, appHeight, memDC, 0, 0, SRCCOPY);
    
//     DeleteObject(memBM);
//     DeleteDC(memDC);

//     //STOP DRAW
// }


// inline void App::Brezenhem(std::vector<DWORD32>& bitmap, glm::ivec2 start, glm::ivec2 end, const Gdiplus::Color& color)
// {
//     if(start.x < 0 || start.x >= appWidht || start.y < 0 || start.y >= appHeight)
//         return;
//     if(end.x < 0 || end.x >= appWidht || end.y < 0 || end.y >= appHeight)
//         return;


//     int dx = std::max(abs(end.x - start.x), 1);
//     int dy = std::max(abs(end.y - start.y), 1);
//     int sx = (end.x - start.x) / dx;
//     int sy = (end.y - start.y) / dy;
//     int err = dx - dy;
    
//     while (start.x != end.x || start.y != end.y)
//     {
//         //std::lock_guard<std::mutex> guard(g_pages_mutex);
//         bitmap[start.y * appWidht + start.x] = color.GetValue();


//         int e2 = err << 1;
//         if (e2 > -dy)
//         {
//             err -= dy;
//             start.x += sx;
//         }
//         if (e2 < dx)
//         {
//             err += dx;
//             start.y += sy;
//         }
//     }
// }
