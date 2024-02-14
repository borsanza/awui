#pragma once

#include <awui/Windows/Forms/JoystickEventArgs.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class JoystickDpadEventArgs : public JoystickEventArgs {
				private:
					int m_dpad;
					int m_value;

				public:
					JoystickDpadEventArgs(int which, int hat, int value);
					virtual ~JoystickDpadEventArgs();

					virtual bool IsClass(Classes objectClass) const override;

					int GetDpad() { return m_dpad; }
					int GetValue() { return m_value; }
			};
		}
	}
}
