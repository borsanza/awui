#pragma once

#include <awui/Object.h>

namespace awui::Windows::Forms {
	class Form;

	class Application : public Object {
		static int quit;

	  private:
		static void ProcessEvents();

	  public:
		Application();

		static void Run(Form *form);
		static int Test();

		virtual bool IsClass(Classes objectClass) const override;

		static void Quit();
	};
} // namespace awui::Windows::Forms
