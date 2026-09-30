#pragma once

#include <awui/Windows/Forms/Events/EventArgs.h>

namespace awui::Windows::Forms::Events {
	class JoystickEventArgs : public EventArgs {
	  private:
		int m_which;

	  public:
		JoystickEventArgs(int which);
		virtual ~JoystickEventArgs() = default;

		int GetWhich() const { return m_which; };
	};
} // namespace awui::Windows::Forms::Events
