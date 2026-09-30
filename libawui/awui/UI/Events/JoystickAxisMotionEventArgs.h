#pragma once

#include <awui/UI/Events/JoystickEventArgs.h>
#include <cstdint>

namespace awui::UI::Events {
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
} // namespace awui::UI::Events
