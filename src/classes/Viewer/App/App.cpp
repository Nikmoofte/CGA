#include "App.hpp"

#include "config.hpp"

#include "Viewer/Window/Window.hpp"
#include "Viewer/Menu/Menu.hpp"
#include "Viewer/ShaderProg/ShaderProg.hpp"

#include "Engine/Settings.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Camera/Camera.hpp"
#include "Engine/Scene/Scene.hpp"
#include "Assets/Mesh/Mesh.hpp"
#include "Engine/Buffers/ColorBuffer/ColorBuffer.hpp"
#include "Engine/Buffers/DepthBuffer/DepthBuffer.hpp"
#include "Engine/Settings.hpp"

#include "path.hpp"

#include <chrono>

std::chrono::time_point<std::chrono::steady_clock> start;
long currentFrame = 0;

namespace Viewer
{

    App::App()
    {    
        CreateWindow();
        CreateRender();
        CreatePipeline();

        RegisterCallbacks();
    }

    App::~App() = default;

    int App::Run()
    {
        start = std::chrono::steady_clock::now();
        while (!window->shouldClose())
        {
            scene->getCamera().OnBeforeRender();
            glClear(GL_COLOR_BUFFER_BIT);
            
            DrawQuad();
            menu->Render();
            renderer->render();
            
            window->swapBuffers();
            window->pollEvents();

            MeasureTime();
        }
        return 0;
    }

    void App::CreateWindow()
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
            "}\n"
        ;
        
        std::string frag = 
            "#version 460 core\n"
            "in vec2 fragmentTexCoords;\n"
            "uniform sampler2D frameBuffer;\n"
            "out vec4 finalColor;\n"
            "void main()\n"
            "{\n"
                "finalColor = texture(frameBuffer, fragmentTexCoords);\n"
            "}\n"
        ;
        auto settings = Engine::Settings::Get();

        window = std::make_unique<Window>(settings.windowSize);
        menu = std::make_unique<Menu>(*window);
        shader = std::make_unique<ShaderProg>();
        shader->create(vert, frag);
        shader->Use();
    }

    void App::RegisterCallbacks()
	{
		window->addOnCursorPositionChanged([this](const double xpos, const double ypos)-> void
		{
			if (menu->WantCaptureKeyboard() || menu->WantCaptureMouse())
				return;

			if (scene->getCamera().OnCursorPositionChanged(xpos, ypos)) {}
		});

		window->addOnKeyChanged([this](const int key, const int scancode, const int action, const int mods)-> void
		{
			if (menu->WantCaptureKeyboard())
				return;

			scene->getCamera().OnKeyChanged(key, scancode, action, mods);
		});

		window->addOnMouseButtonChanged([this](const int button, const int action, const int mods)-> void
		{
			if (menu->WantCaptureMouse())
				return;

			scene->getCamera().OnMouseButtonChanged(button, action, mods);
		});

		window->addOnScrollChanged([this](const double xoffset, const double yoffset)-> void { });

        window->addOnResize([this](const int width, const int height)-> void
        {
            Resize(width, height);
        });

	}

    void App::MeasureTime() const
    {
        ++currentFrame;
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> delta = end - start;

        if(delta.count() >= 1.0)
        {
            const double fFPS = (double)currentFrame / delta.count();
            start = end;
            currentFrame = 0;
            std::string title = "OpenGL - " + std::to_string(fFPS) + " fps";
            window->setTitle(title.c_str());
        }
    }

    void App::Resize(int width, int height)
    {
        glViewport(0, 0, width, height);
        auto& settings = Engine::Settings::Get();
        settings.windowSize = {width, height};
        scene->getCamera().resize(settings.fov, settings.windowSize);
        renderer->resetBuffers();
    }

    void App::DrawQuad() const
    {
        auto& screenDimentions = Engine::Settings::Get().windowSize;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenDimentions.x, screenDimentions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, renderer->getColorBuffer());
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void App::CreatePipeline()
    {
        float vertex[] =
        {
            1.0f,  1.0f, 1.0f, 0.0f, //top right
            -1.0f,  1.0f, 0.0f, 0.0f, //top left
            -1.0f, -1.0f, 0.0f, 1.0f, //bottom left

            -1.0f, -1.0f, 0.0f, 1.0f, //bottom left
            1.0f, -1.0f, 1.0f, 1.0f, //bottom right
            1.0f,  1.0f, 1.0f, 0.0f, //top right
        };

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void App::CreateRender()
    {
        scene = std::make_unique<Engine::Scene>();
        renderer = std::make_unique<Engine::Renderer>(*scene);
    }
}