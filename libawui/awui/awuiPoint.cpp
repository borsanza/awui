// (c) Copyright 2011 Borja S�nchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiPoint.h"
#include "awuiSize.h"

awuiPoint::awuiPoint() {
	this->x = 0;
	this->y = 0;
}

awuiPoint::awuiPoint(const awuiSize sz) {
	this->x = sz.GetWidth();
	this->y = sz.GetHeight();
}

awuiPoint::awuiPoint(int x, int y) {
	this->x = x;
	this->y = y;
}

int awuiPoint::GetX() const {
	return this->x;
}

void awuiPoint::SetX(int x) {
	this->x = x;
}

int awuiPoint::GetY() const {
	return this->y;
}

void awuiPoint::SetY(int y) {
	this->y = y;
}

awuiPoint & awuiPoint::operator= (const awuiPoint & other) {
	this->x = other.x;
	this->y = other.y;

	return *this;
}