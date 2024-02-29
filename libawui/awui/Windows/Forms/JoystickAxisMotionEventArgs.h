#pragma once

#include <awui/Windows/Forms/JoystickEventArgs.h>
#include <cstdint>

namespace awui {
	namespace Windows {
		namespace Forms {
			class JoystickAxisMotionEventArgs : public JoystickEventArgs {
				private:
					int16_t m_axisX;
					int16_t m_axisY;

				public:
					JoystickAxisMotionEventArgs(int which, int16_t axisX, int16_t axisY);
					virtual ~JoystickAxisMotionEventArgs();

					virtual bool IsClass(Classes objectClass) const override;

					int16_t GetAxisX() { return m_axisX; }
					int16_t GetAxisY() { return m_axisY; }
			};
		}
	}
}
