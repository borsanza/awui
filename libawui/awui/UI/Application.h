#pragma once

#include <awui/Object.h>

namespace awui::UI {
	class Form;

	class Application : public Object {
		static int s_quit;

	  private:
		static void ProcessEvents();

	  public:
		Application();

		static void Run(Form *form);

		static void Quit();
	};
} // namespace awui::UI
