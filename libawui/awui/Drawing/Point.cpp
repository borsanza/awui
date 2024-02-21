/*
 * awui/Drawing/Point.cpp
 *
 * Copyright (C) 2011 Borja Sánchez Zamorano
 */

#include "Point.h"

#include <awui/Convert.h>
#include <awui/Drawing/Size.h>
#include <awui/Math.h>
#include <awui/String.h>

using namespace awui::Drawing;

Point::Point() {
	m_x = 0.0f;
	m_y = 0.0f;
}

Point::Point(const Size sz) {
	m_x = sz.GetWidth();
	m_y = sz.GetHeight();
}

Point::Point(float x, float y) {
	m_x = x;
	m_y = y;
}

Point::~Point() {
}

void Point::SetX(float x) {
	m_x = x;
}

void Point::SetY(float y) {
	m_y = y;
}

Point & Point::operator= (const Point & other) {
	m_x = other.m_x;
	m_y = other.m_y;

	return *this;
}

awui::String Point::ToString() {
	String value;
	value = String("{X=") + Convert::ToString(m_x) + ",Y=" + Convert::ToString(m_y) + "}";
	return value;
}

float Point::Distance(Point * p1, Point * p2) {
	return Math::Sqrt(Math::Pow((float)p1->GetX() - p2->GetX(), 2.0f)
					+ Math::Pow((float)p1->GetY() - p2->GetY(), 2.0f));
}
