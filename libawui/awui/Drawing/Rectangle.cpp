/*
 * awui/Drawing/Rectangle.cpp
 *
 * Copyright (C) 2011 Borja Sánchez Zamorano
 */

#include "Rectangle.h"

#include <awui/Convert.h>
#include <awui/Drawing/Point.h>
#include <awui/Drawing/Size.h>
#include <awui/String.h>

using namespace awui::Drawing;

Rectangle::Rectangle() {
	this->location = Point(0, 0);
	this->size = Size(0, 0);
}

Rectangle::Rectangle(const Point location, const Size size) {
	this->location = location;
	this->size = size;
}

Rectangle::Rectangle(int x, int y, int width, int height) {
	this->location = Point(x, y);
	this->size = Size(width, height);
}

int Rectangle::GetWidth() const {
	return this->size.GetWidth();
}

void Rectangle::SetWidth(int width) {
	this->size.SetWidth(width);
}

int Rectangle::GetHeight() const {
	return this->size.GetHeight();
}

void Rectangle::SetHeight(int height) {
	this->size.SetHeight(height);
}

void Rectangle::SetX(int x) {
	this->location.SetX(x);
}

void Rectangle::SetY(int y) {
	this->location.SetY(y);
}

int Rectangle::GetBottom() const {
	return this->GetY() + this->GetHeight() - 1;
}

int Rectangle::GetRight() const {
	return this->GetX() + this->GetWidth() - 1;
}

Point Rectangle::GetLocation() const {
	return this->location;
}

Size Rectangle::GetSize() const {
	return this->size;
}

void Rectangle::SetLocation(Point location) {
	this->location = location;
}

void Rectangle::SetSize(Size size) {
	this->size = size;
}

void Rectangle::Inflate(Size size) {
	this->Inflate(size.GetWidth(), size.GetHeight());
}

void Rectangle::Inflate(int width, int height) {
	this->SetWidth(this->GetWidth() + width);
	this->SetHeight(this->GetHeight() + height);
}

void Rectangle::Offset(const Point pos) {
	this->Offset(pos.GetX(), pos.GetY());
}

void Rectangle::Offset(int x, int y) {
	this->SetX(this->GetX() + x);
	this->SetY(this->GetY() + y);
}

Rectangle & Rectangle::operator=(const Rectangle & other) {
	this->size = other.size;
	this->location = other.location;

	return *this;
}

Rectangle Rectangle::FromLTRB(int left, int top, int right, int bottom) {
 	return Rectangle(left, top, right - left + 1, bottom - top + 1);
}

Rectangle Rectangle::Intersect(const Rectangle rectangle1, const Rectangle rectangle2) {
	int left   = rectangle1.GetLeft() > rectangle2.GetLeft()?  rectangle1.GetLeft() : rectangle2.GetLeft();
	int top    = rectangle1.GetTop() > rectangle2.GetTop()? rectangle1.GetTop() : rectangle2.GetTop();
	int right  = rectangle1.GetRight() < rectangle2.GetRight()? rectangle1.GetRight() : rectangle2.GetRight();
	int bottom = rectangle1.GetBottom() < rectangle2.GetBottom()? rectangle1.GetBottom(): rectangle2.GetBottom();

	return Rectangle::FromLTRB(left, top, right, bottom);
}

void Rectangle::Intersect(const Rectangle rectangle) {
	*this = Rectangle::Intersect(*this, rectangle);
}

awui::String Rectangle::ToString() {
	String value;
	value = String("{X=") + Convert::ToString(this->location.GetX()) + ",Y=" + Convert::ToString(this->location.GetY()) + ",Width=" + Convert::ToString(this->size.GetWidth()) + ", Height=" + Convert::ToString(this->size.GetHeight()) + "}";
	return value;
}