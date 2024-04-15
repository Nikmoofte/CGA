#ifndef APP_HPP
#define APP_HPP

#include <memory>

namespace Engine
{
	class Renderer;
	class Scene;
}

namespace Viewer
{
	class App 
	{
	public:
		App();
		~App();

		int Run();

	private:
		void DrawQuad() const;
		void CreatePipeline();
		void CreateRender();
		void CreateWindow();
		void RegisterCallbacks();
		void MeasureTime() const;
		void Resize(int width, int heigh);
		void UpdateSettings();
		void Recreate();

		// OpenGL handlers
		unsigned vbo{};
        unsigned vao{};
        unsigned texture{};

		// Viewer
		std::unique_ptr<class Window> window;
		std::unique_ptr<class Menu> menu;
		std::unique_ptr<class ShaderProg> shader;

		// Engine
		std::unique_ptr<Engine::Renderer> renderer;
		std::unique_ptr<Engine::Scene> scene;
	};
}

#endif