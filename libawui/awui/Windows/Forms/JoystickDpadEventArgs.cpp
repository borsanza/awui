// (c) Copyright 2024 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "JoystickDpadEventArgs.h"

using namespace awui::Windows::Forms;

JoystickDpadEventArgs::JoystickDpadEventArgs(int which, int dpad, int value) : JoystickEventArgs(which) {
	m_dpad = dpad;
	m_value = value;
}

JoystickDpadEventArgs::~JoystickDpadEventArgs() {
}

bool JoystickDpadEventArgs::IsClass(Classes objectClass) const {
	if (objectClass == Classes::JoystickMoveEventArgs) {
		return true;
	}

	return Object::IsClass(objectClass);
}
