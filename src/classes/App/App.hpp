#ifndef APP_HPP
#define APP_HPP

#include <string>
#include <exception>
#include <chrono>
#include <thread>
#include <memory>
#define GLM_FORCE_SSE42
#include <glm/glm.hpp>

#include "Object/Object.hpp"
#include "Camera/Camera.hpp"
#include "Renderer/Renderer.hpp"

class App
{
public:
    explicit App(size_t width, size_t height);
    explicit App(const std::string& filePath);

    ~App();

    int run();
private:
    GLFWwindow* makeWindow();
    void calculateFPS();
    void resize();

    void processInput(GLFWwindow* window);
    size_t appWidth;
    size_t appHeight;

    Camera camera{glm::vec3(-10.0f, 0.0f, 0.0f), 1920, 1080};
    std::unique_ptr<Renderer> renderer;
    BS::thread_pool threads{8};


    GLFWwindow* window;
    std::chrono::time_point<std::chrono::system_clock> frameStart;
    std::chrono::duration<double> delta;
    Object obj; 

};

#endif