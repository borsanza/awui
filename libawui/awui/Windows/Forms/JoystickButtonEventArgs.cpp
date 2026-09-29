// (c) Copyright 2024 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "JoystickButtonEventArgs.h"

using namespace awui::Windows::Forms;

JoystickButtonEventArgs::JoystickButtonEventArgs(int which, int button, uint32_t buttons, uint32_t prevButtons) : JoystickEventArgs(which) {
	m_button = button;
	m_buttons = buttons;
	m_prevButtons = prevButtons;
}

