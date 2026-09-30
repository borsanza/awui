// (c) Copyright 2024 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "JoystickAxisMotionEventArgs.h"

using namespace awui::Windows::Forms;

JoystickAxisMotionEventArgs::JoystickAxisMotionEventArgs(int which, int16_t axisX, int16_t axisY) : JoystickEventArgs(which) {
	m_axisX = axisX;
	m_axisY = axisY;
}

