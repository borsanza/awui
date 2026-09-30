
#pragma once

#include <awui/Object.h>

namespace awui::Windows::Forms::Events {
	class IRemoteListener {
	  public:
		virtual void OnOk(Control *sender) = 0;
		virtual void OnMenu(Control *sender) = 0;
	};
} // namespace awui::Windows::Forms::Events
