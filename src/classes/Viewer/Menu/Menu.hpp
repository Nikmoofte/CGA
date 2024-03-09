#ifndef MENU_HPP
#define MENU_HPP

#include <imgui.h>

//#include "../Engine/Settings.h"

namespace Viewer
{
	class Menu final
	{
	public:
		Menu(const class Window& window);
		~Menu();

		void Render();

		static bool WantCaptureMouse()
		{
			return ImGui::GetIO().WantCaptureMouse;
		}

		static bool WantCaptureKeyboard()
		{
			return ImGui::GetIO().WantCaptureKeyboard;
		}

	private:
	};
}

#endif