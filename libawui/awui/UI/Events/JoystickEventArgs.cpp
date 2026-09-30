// (c) Copyright 2024 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "JoystickEventArgs.h"

using namespace awui::UI::Events;

JoystickEventArgs::JoystickEventArgs(int which) {
	m_which = which;
}

