#pragma once

#include <awui/Windows/Forms/JoystickEventArgs.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class JoystickButtonEventArgs : public JoystickEventArgs {
				private:
					int m_button;
					uint32_t m_buttons;
					uint32_t m_prevButtons;

				public:
					JoystickButtonEventArgs(int which, int button, uint32_t buttons, uint32_t prevButtons);
					virtual ~JoystickButtonEventArgs();

					virtual bool IsClass(Classes objectClass) const override;

					int GetButton() { return m_button; }
					int GetButtons() { return m_buttons; }
			};
		}
	}
}
