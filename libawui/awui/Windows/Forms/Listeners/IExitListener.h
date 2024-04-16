
#pragma once

#include <awui/Object.h>

namespace awui::Windows::Forms::Listeners {
	class IExitListener {
	  public:
		virtual void OnExit(Control *sender) = 0;
	};
} // namespace awui::Windows::Forms::Listeners
