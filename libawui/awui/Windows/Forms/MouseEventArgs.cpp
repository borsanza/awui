// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "MouseEventArgs.h"

using namespace awui::Windows::Forms;

MouseEventArgs::MouseEventArgs() {
	m_class = Classes::MouseEventArgs;
	m_x = 0;
	m_y = 0;
	m_delta = 0;
	m_clicks = 0;
	m_button = 0;
}

bool MouseEventArgs::IsClass(Classes objectClass) const {
	return (objectClass == Classes::MouseEventArgs) || EventArgs::IsClass(objectClass);
}

int MouseEventArgs::GetX() {
	return m_x;
}

void MouseEventArgs::SetX(int x) {
	m_x = x;
}

int MouseEventArgs::GetY() {
	return m_y;
}

void MouseEventArgs::SetY(int y) {
	m_y = y;
}

void MouseEventArgs::GetLocation(int &x, int &y) {
	x = m_x;
	y = m_y;
}

void MouseEventArgs::SetLocation(int x, int y) {
	m_x = x;
	m_y = y;
}

int MouseEventArgs::GetDelta() {
	return m_delta;
}

void MouseEventArgs::SetDelta(int delta) {
	m_delta = delta;
}

int MouseEventArgs::GetClicks() {
	return m_clicks;
}

void MouseEventArgs::SetClicks(int clicks) {
	m_clicks = clicks;
}

int MouseEventArgs::GetButton() {
	return m_button;
}

void MouseEventArgs::SetButton(int button) {
	m_button = button;
}
