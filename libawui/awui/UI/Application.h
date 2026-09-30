#pragma once

namespace awui::UI {
	class Form;

	class Application {
		static int s_quit;

	  private:
		static void ProcessEvents();

	  public:
		Application() = delete; // Solo métodos estáticos

		static void Run(Form *form);

		static void Quit();
	};
} // namespace awui::UI
