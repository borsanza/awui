/*
 * awui/Drawing/Point.cpp
 *
 * Copyright (C) 2011 Borja Sánchez Zamorano
 */

#include "Point.h"

#include <awui/Convert.h>
#include <awui/Drawing/Size.h>
#include <awui/String.h>

using namespace awui::Drawing;

Point::Point() {
	this->_x = 0;
	this->_y = 0;
}

Point::Point(const Size sz) {
	this->_x = sz.GetWidth();
	this->_y = sz.GetHeight();
}

Point::Point(int x, int y) {
	this->_x = x;
	this->_y = y;
}

void Point::SetX(int x) {
	this->_x = x;
}

void Point::SetY(int y) {
	this->_y = y;
}

Point & Point::operator= (const Point & other) {
	this->_x = other._x;
	this->_y = other._y;

	return *this;
}

awui::String Point::ToString() {
	String value;
	value = String("{X=") + Convert::ToString(this->_x) + ",Y=" + Convert::ToString(this->_y) + "}";
	return value;
}


int Point::Sign(Point * p1, Point * p2, Point * p3) {
	return (p1->_x - p3->_x) * (p2->_y - p3->_y) - (p2->_x - p3->_x) * (p1->_y - p3->_y);
}

bool Point::InTriangle(Point * v1, Point * v2, Point * v3) {
	bool b1, b2, b3;

	b1 = Point::Sign(this, v1, v2) < 0;
	b2 = Point::Sign(this, v2, v3) < 0;
	b3 = Point::Sign(this, v3, v1) < 0;

	return ((b1 == b2) && (b2 == b3));
}