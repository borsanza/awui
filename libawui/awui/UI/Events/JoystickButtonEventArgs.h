#pragma once

#include <awui/UI/Events/JoystickEventArgs.h>
#include <cstdint>

namespace awui::UI::Events {
	class JoystickButtonEventArgs : public JoystickEventArgs {
	  private:
		int m_button;
		uint32_t m_buttons;
		uint32_t m_prevButtons;

	  public:
		JoystickButtonEventArgs(int which, int button, uint32_t buttons, uint32_t prevButtons);
		virtual ~JoystickButtonEventArgs() = default;

		int GetButton() const { return m_button; }
		int GetButtons() const { return m_buttons; }
	};
} // namespace awui::UI::Events
