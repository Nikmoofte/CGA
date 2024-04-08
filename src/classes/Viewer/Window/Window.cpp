#include "Window.hpp"

#include <stdexcept>
#include <iostream>

namespace Viewer
{
    Window::Window(int width, int height) : width(width), height(height)
    {
        if (!glfwInit())
		{
			throw std::runtime_error("glfwInit() failed!");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width, height, "CPU Rasterizer", nullptr, nullptr);

		if (window == nullptr)
		{
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			throw std::runtime_error("Failed to initialize GLAD");
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, GlfwKeyCallback);
		glfwSetCursorPosCallback(window, GlfwCursorPositionCallback);
		glfwSetMouseButtonCallback(window, GlfwMouseButtonCallback);
		glfwSetScrollCallback(window, GlfwScrollCallback);
		glfwSetWindowSizeCallback(window, GlfwResizeCallBack);
        glViewport(0, 0, width, height);
    }

    Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

    void Window::GlfwResizeCallBack(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void Window::GlfwErrorCallback(const int error, const char* const description)
	{
		std::cerr << "GLFW error code " << error << ": " << description << std::endl;
	}

	void Window::GlfwKeyCallback(GLFWwindow* window, const int key, const int scancode, const int action,
	                             const int mods)
	{
		auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : this_->onKeyChanged)
			callback(key, scancode, action, mods);
	}

	void Window::GlfwCursorPositionCallback(GLFWwindow* window, const double xpos, const double ypos)
	{
		auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : this_->onCursorPositionChanged)
			callback(xpos, ypos);
	}

	void Window::GlfwMouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
	{
		auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : this_->onMouseButtonChanged)
			callback(button, action, mods);
	}

	void Window::GlfwScrollCallback(GLFWwindow* window, const double xoffset, const double yoffset)
	{
		auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : this_->onScrollChanged)
			callback(xoffset, yoffset);
	}

    void Window::AddOnKeyChanged(std::function<void(int key, int scancode, int action, int mods)>&& callback)
    {
        onKeyChanged.push_back(callback);
    }

    void Window::AddOnCursorPositionChanged(std::function<void(double xpos, double ypos)>&& callback)
    {
        onCursorPositionChanged.push_back(callback);
    }

    void Window::AddOnMouseButtonChanged(std::function<void(int button, int action, int mods)>&& callback)
    {
        onMouseButtonChanged.push_back(callback);
    }

    void Window::AddOnScrollChanged(std::function<void(double xoffset, double yoffset)>&& callback)
    {
        onScrollChanged.push_back(callback);
    }

}