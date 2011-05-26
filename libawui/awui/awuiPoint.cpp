// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiPoint.h"

int awuiPoint::GetX() {
	return this->x;
}

void awuiPoint::SetX(int x) {
	this->x = x;
}

int awuiPoint::GetY() {
	return this->y;
}

void awuiPoint::SetY(int y) {
	this->y = y;
}

awuiPoint & awuiPoint::operator= (const awuiPoint & other) {
	x = other.x;
	y = other.y;

	return *this;
}