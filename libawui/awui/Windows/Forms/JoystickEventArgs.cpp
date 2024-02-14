// (c) Copyright 2024 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "JoystickEventArgs.h"

using namespace awui::Windows::Forms;

JoystickEventArgs::JoystickEventArgs(int which) {
	m_which = which;
}

JoystickEventArgs::~JoystickEventArgs() {
}

bool JoystickEventArgs::IsClass(Classes objectClass) const {
	if (objectClass == Classes::JoystickEventArgs) {
		return true;
	}

	return Object::IsClass(objectClass);
}
