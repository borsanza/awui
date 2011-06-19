// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiMouseEventArgs.h"

awuiMouseEventArgs::awuiMouseEventArgs() {
	this->x = 0;
	this->y = 0;
	this->delta = 0;
	this->clicks = 0;
	this->button = 0;
}

awuiMouseEventArgs::~awuiMouseEventArgs() {
}

int awuiMouseEventArgs::IsClass(awuiObject::awuiClasses objectClass) const {
	if (objectClass == awuiObject::MouseEventArgs)
		return 1;

	return awuiObject::IsClass(objectClass);
}

int awuiMouseEventArgs::GetX() {
	return this->x;
}

void awuiMouseEventArgs::SetX(int x) {
	this->x = x;
}

int awuiMouseEventArgs::GetY() {
	return this->y;
}

void awuiMouseEventArgs::SetY(int y) {
	this->y = y;
}

void awuiMouseEventArgs::GetLocation(int &x, int &y) {
	x = this->x;
	y = this->y;
}

void awuiMouseEventArgs::SetLocation(int x, int y) {
	this->x = x;
	this->y = y;
}

int awuiMouseEventArgs::GetDelta() {
	return this->delta;
}

void awuiMouseEventArgs::SetDelta(int delta) {
	this->delta = delta;
}

int awuiMouseEventArgs::GetClicks() {
	return this->clicks;
}

void awuiMouseEventArgs::SetClicks(int clicks) {
	this->clicks = clicks;
}

int awuiMouseEventArgs::GetButton() {
	return this->button;
}

void awuiMouseEventArgs::SetButton(int button) {
	this->button = button;
}