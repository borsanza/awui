#pragma once

#include <awui/Windows/Forms/EventArgs.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class JoystickEventArgs : public EventArgs {
				private:
					int m_which;

				public:
					JoystickEventArgs(int which);
					virtual ~JoystickEventArgs();

					virtual bool IsClass(Classes objectClass) const override;

					int GetWhich() { return m_which; };
			};
		}
	}
}
