#pragma once

#include <awui/Windows/Forms/EventArgs.h>

namespace awui::Windows::Forms {
	class JoystickEventArgs : public EventArgs {
		private:
			int m_which;

		public:
			JoystickEventArgs(int which);
			virtual ~JoystickEventArgs() = default;

			virtual bool IsClass(Classes objectClass) const override;

			int GetWhich() { return m_which; };
	};
}
