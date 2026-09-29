#pragma once

#include <awui/Windows/Forms/JoystickEventArgs.h>
#include <cstdint>

namespace awui::Windows::Forms {
	class JoystickAxisMotionEventArgs : public JoystickEventArgs {
	  private:
		int16_t m_axisX;
		int16_t m_axisY;

	  public:
		JoystickAxisMotionEventArgs(int which, int16_t axisX, int16_t axisY);
		virtual ~JoystickAxisMotionEventArgs() = default;

		int16_t GetAxisX() { return m_axisX; }
		int16_t GetAxisY() { return m_axisY; }
	};
} // namespace awui::Windows::Forms
