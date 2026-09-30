
#pragma once

#include <awui/Object.h>

namespace awui::UI::Events {
	class IExitListener {
	  public:
		virtual void OnExit(Control *sender) = 0;
	};
} // namespace awui::UI::Events
