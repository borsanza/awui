// (c) Copyright 2011 Borja S�nchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Point.h"

#include <awui/Drawing/Size.h>

using namespace awui::Drawing;

Point::Point() {
	this->x = 0;
	this->y = 0;
}

Point::Point(const Size sz) {
	this->x = sz.GetWidth();
	this->y = sz.GetHeight();
}

Point::Point(int x, int y) {
	this->x = x;
	this->y = y;
}

int Point::GetX() const {
	return this->x;
}

void Point::SetX(int x) {
	this->x = x;
}

int Point::GetY() const {
	return this->y;
}

void Point::SetY(int y) {
	this->y = y;
}

Point & Point::operator= (const Point & other) {
	this->x = other.x;
	this->y = other.y;

	return *this;
}
