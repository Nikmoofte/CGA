#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Viewer
{
    class Window
    {
        public:
            Window(int width, int height);
            ~Window();

            GLFWwindow* Get() const { return window; }
            bool shouldClose() const { return glfwWindowShouldClose(window); }
            void pollEvents() const { glfwPollEvents(); }
            void swapBuffers() const { glfwSwapBuffers(window); }

            void AddOnKeyChanged(std::function<void(int key, int scancode, int action, int mods)>&& callback);
            void AddOnCursorPositionChanged(std::function<void(double xpos, double ypos)>&& callback);
            void AddOnMouseButtonChanged(std::function<void(int button, int action, int mods)>&& callback);
            void AddOnScrollChanged(std::function<void(double xoffset, double yoffset)>&& callback);

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

            GLFWwindow* window{ nullptr };
            const int width{};
            const int height{};
    };
}


#endif