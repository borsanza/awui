// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "MouseEventArgs.h"

using namespace awui::Windows::Forms;

MouseEventArgs::MouseEventArgs() {
	this->x = 0;
	this->y = 0;
	this->delta = 0;
	this->clicks = 0;
	this->button = 0;
}

MouseEventArgs::~MouseEventArgs() {
}

bool MouseEventArgs::IsClass(Classes objectClass) const {
	if (objectClass == Classes::MouseEventArgs) {
		return true;
	}

	return Object::IsClass(objectClass);
}

int MouseEventArgs::GetX() {
	return this->x;
}

void MouseEventArgs::SetX(int x) {
	this->x = x;
}

int MouseEventArgs::GetY() {
	return this->y;
}

void MouseEventArgs::SetY(int y) {
	this->y = y;
}

void MouseEventArgs::GetLocation(int &x, int &y) {
	x = this->x;
	y = this->y;
}

void MouseEventArgs::SetLocation(int x, int y) {
	this->x = x;
	this->y = y;
}

int MouseEventArgs::GetDelta() {
	return this->delta;
}

void MouseEventArgs::SetDelta(int delta) {
	this->delta = delta;
}

int MouseEventArgs::GetClicks() {
	return this->clicks;
}

void MouseEventArgs::SetClicks(int clicks) {
	this->clicks = clicks;
}

int MouseEventArgs::GetButton() {
	return this->button;
}

void MouseEventArgs::SetButton(int button) {
	this->button = button;
}
