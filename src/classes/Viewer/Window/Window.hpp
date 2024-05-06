#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Viewer
{
    class Window
    {
        public:
            Window(int width, int height);
            Window(glm::ivec2 size) : Window(size.x, size.y) {}
            ~Window();

            GLFWwindow* Get() const { return window; }
            bool shouldClose() const { return glfwWindowShouldClose(window); }
            void pollEvents() const { glfwPollEvents(); }
            void swapBuffers() const { glfwSwapBuffers(window); }
            void setTitle(const char* title) const { glfwSetWindowTitle(window, title); }
            void setWindowShouldClose(bool should) {glfwSetWindowShouldClose(window, should);}

            void addOnKeyChanged(std::function<void(int key, int scancode, int action, int mods)>&& callback);
            void addOnCursorPositionChanged(std::function<void(double xpos, double ypos)>&& callback);
            void addOnMouseButtonChanged(std::function<void(int button, int action, int mods)>&& callback);
            void addOnScrollChanged(std::function<void(double xoffset, double yoffset)>&& callback);
            void addOnResize(std::function<void(int width, int height)>&& callback);
            
        private:
            static void GlfwErrorCallback(int error, const char* description);
            static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void GlfwCursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
            static void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
            static void GlfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
            static void GlfwResizeCallBack(GLFWwindow* window, int width, int height);

            std::vector<std::function<void(int key, int scancode, int action, int mods)>> onKeyChanged;
            std::vector<std::function<void(double xpos, double ypos)>> onCursorPositionChanged;
            std::vector<std::function<void(int button, int action, int mods)>> onMouseButtonChanged;
            std::vector<std::function<void(double xoffset, double yoffset)>> onScrollChanged;
            std::vector<std::function<void(int width, int height)>> onResize;

            GLFWwindow* window{ nullptr };
            const int width{};
            const int height{};
    };
}


#endif