#pragma once

#include <awui/UI/Events/EventArgs.h>

namespace awui::UI::Events {
	class JoystickEventArgs : public EventArgs {
	  private:
		int m_which;

	  public:
		JoystickEventArgs(int which);
		virtual ~JoystickEventArgs() = default;

		int GetWhich() const { return m_which; };
	};
} // namespace awui::UI::Events
