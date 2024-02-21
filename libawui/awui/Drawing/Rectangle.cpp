/*
 * awui/Drawing/Rectangle.cpp
 *
 * Copyright (C) 2011 Borja Sanchez Zamorano
 */

#include "Rectangle.h"

#include <awui/Convert.h>
#include <awui/Drawing/Point.h>
#include <awui/Drawing/Size.h>
#include <awui/String.h>

using namespace awui::Drawing;

Rectangle::Rectangle() {
	m_location = Point(0.0f, 0.0f);
	m_size = Size(0.0f, 0.0f);
}

Rectangle::Rectangle(const Point& location, const Size& size) {
	m_location = location;
	m_size = size;
}

Rectangle::Rectangle(float x, float y, float width, float height) {
	m_location = Point(x, y);
	m_size = Size(width, height);
}

Rectangle::~Rectangle() {
}

void Rectangle::SetWidth(float width) {
	m_size.SetWidth(width);
}

void Rectangle::SetHeight(float height) {
	m_size.SetHeight(height);
}

void Rectangle::SetX(float x) {
	m_location.SetX(x);
}

void Rectangle::SetY(float y) {
	m_location.SetY(y);
}

float Rectangle::GetBottom() const {
	return GetY() + GetHeight() - 1.0f;
}

float Rectangle::GetRight() const {
	return GetX() + GetWidth() - 1.0f;
}

Point Rectangle::GetLocation() const {
	return m_location;
}

Size Rectangle::GetSize() const {
	return m_size;
}

void Rectangle::SetLocation(const Point& location) {
	m_location = location;
}

void Rectangle::SetSize(const Size& size) {
	m_size = size;
}

void Rectangle::SetSize(float width, float height) {
	m_size = Size(width, height);
}

void Rectangle::Inflate(const Size& size) {
	Inflate(size.GetWidth(), size.GetHeight());
}

void Rectangle::Inflate(float width, float height) {
	SetWidth(GetWidth() + width);
	SetHeight(GetHeight() + height);
}

void Rectangle::Offset(const Point& pos) {
	Offset(pos.GetX(), pos.GetY());
}

void Rectangle::Offset(float x, float y) {
	SetX(GetX() + x);
	SetY(GetY() + y);
}

Rectangle & Rectangle::operator=(const Rectangle & other) {
	m_size = other.m_size;
	m_location = other.m_location;

	return *this;
}

Rectangle Rectangle::FromLTRB(float left, float top, float right, float bottom) {
 	return Rectangle(left, top, right - left + 1.0f, bottom - top + 1.0f);
}

Rectangle Rectangle::Intersect(const Rectangle& rectangle1, const Rectangle& rectangle2) {
	float left   = rectangle1.GetLeft() > rectangle2.GetLeft()?  rectangle1.GetLeft() : rectangle2.GetLeft();
	float top    = rectangle1.GetTop() > rectangle2.GetTop()? rectangle1.GetTop() : rectangle2.GetTop();
	float right  = rectangle1.GetRight() < rectangle2.GetRight()? rectangle1.GetRight() : rectangle2.GetRight();
	float bottom = rectangle1.GetBottom() < rectangle2.GetBottom()? rectangle1.GetBottom(): rectangle2.GetBottom();

	return Rectangle::FromLTRB(left, top, right, bottom);
}

void Rectangle::Intersect(const Rectangle& rectangle) {
	*this = Rectangle::Intersect(*this, rectangle);
}

awui::String Rectangle::ToString() {
	String value;
	value = String("{X=") + Convert::ToString(m_location.GetX()) + ",Y=" + Convert::ToString(m_location.GetY()) + ",Width=" + Convert::ToString(m_size.GetWidth()) + ", Height=" + Convert::ToString(m_size.GetHeight()) + "}";
	return value;
}
