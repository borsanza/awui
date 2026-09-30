#pragma once

namespace awui::UI::Events {
	class EventArgs {
	  public:
		EventArgs();
		virtual ~EventArgs() = default; // Base de MouseEventArgs y JoystickEventArgs
	};
} // namespace awui::UI::Events
