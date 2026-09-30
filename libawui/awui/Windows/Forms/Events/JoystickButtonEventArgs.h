#pragma once

#include <awui/Windows/Forms/Events/JoystickEventArgs.h>
#include <cstdint>

namespace awui::Windows::Forms::Events {
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
} // namespace awui::Windows::Forms::Events
