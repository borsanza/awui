// (c) Copyright 2011 Borja S�nchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Point.h"

#include <awui/Convert.h>
#include <awui/Drawing/Size.h>
#include <awui/String.h>

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

awui::String Point::ToString() {
	String value;
	value = String("{X=") + Convert::ToString(this->x) + ",Y=" + Convert::ToString(this->y) + "}";
	return value;
}


int Point::Sign(Point * p1, Point * p2, Point * p3) {
  return (p1->x - p3->x) * (p2->y - p3->y) - (p2->x - p3->x) * (p1->y - p3->y);
}

bool Point::InTriangle(Point * v1, Point * v2, Point * v3) {
  bool b1, b2, b3;

  b1 = Point::Sign(this, v1, v2) < 0;
  b2 = Point::Sign(this, v2, v3) < 0;
  b3 = Point::Sign(this, v3, v1) < 0;

  return ((b1 == b2) && (b2 == b3));
}
