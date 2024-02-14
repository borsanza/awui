#pragma once

#include <awui/Windows/Forms/JoystickEventArgs.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class JoystickButtonEventArgs : public JoystickEventArgs {
				private:
					int m_button;

				public:
					JoystickButtonEventArgs(int which, int button);
					virtual ~JoystickButtonEventArgs();

					virtual bool IsClass(Classes objectClass) const override;

					int GetButton() { return m_button; }
			};
		}
	}
}
