// (c) Copyright 2024 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "JoystickEventArgs.h"

using namespace awui::Windows::Forms;

JoystickEventArgs::JoystickEventArgs(int which) {
	m_class = Classes::JoystickEventArgs;
	m_which = which;
}

bool JoystickEventArgs::IsClass(Classes objectClass) const {
	return (objectClass == Classes::JoystickEventArgs) || EventArgs::IsClass(objectClass);
}
