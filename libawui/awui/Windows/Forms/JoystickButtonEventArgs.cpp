// (c) Copyright 2024 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "JoystickButtonEventArgs.h"

using namespace awui::Windows::Forms;

JoystickButtonEventArgs::JoystickButtonEventArgs(int which, int button) : JoystickEventArgs(which) {
	m_button = button;
}

JoystickButtonEventArgs::~JoystickButtonEventArgs() {
}

bool JoystickButtonEventArgs::IsClass(Classes objectClass) const {
	if (objectClass == Classes::JoystickButtonEventArgs) {
		return true;
	}

	return Object::IsClass(objectClass);
}
