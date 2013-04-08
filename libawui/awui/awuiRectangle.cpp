// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiRectangle.h"

#include "awuiPoint.h"
#include "awuiSize.h"

awuiRectangle::awuiRectangle() {
	this->location = awuiPoint(0, 0);
	this->size = awuiSize(0, 0);
}

awuiRectangle::awuiRectangle(const awuiPoint location, const awuiSize size) {
	this->location = location;
	this->size = size;
}

awuiRectangle::awuiRectangle(int x, int y, int width, int height) {
	this->location = awuiPoint(x, y);
	this->size = awuiSize(width, height);
}

int awuiRectangle::GetWidth() const {
	return this->size.GetWidth();
}

void awuiRectangle::SetWidth(int width) {
	this->size.SetWidth(width);
}

int awuiRectangle::GetHeight() const {
	return this->size.GetHeight();
}

void awuiRectangle::SetHeight(int height) {
	this->size.SetHeight(height);
}

int awuiRectangle::GetX() const {
	return this->location.GetX();
}

void awuiRectangle::SetX(int x) {
	this->location.SetX(x);
}

int awuiRectangle::GetY() const {
	return this->location.GetY();
}

void awuiRectangle::SetY(int y) {
	this->location.SetY(y);
}

int awuiRectangle::GetBottom() const {
	return this->GetY() + this->GetHeight() - 1;
}

int awuiRectangle::GetLeft() const {
	return this->GetX();
}

int awuiRectangle::GetRight() const {
	return this->GetX() + this->GetWidth() - 1;
}

int awuiRectangle::GetTop() const {
	return this->GetY();
}

awuiPoint awuiRectangle::GetLocation() const {
	return this->location;
}

awuiSize awuiRectangle::GetSize() const {
	return this->size;
}

void awuiRectangle::SetLocation(awuiPoint location) {
	this->location = location;
}

void awuiRectangle::SetSize(awuiSize size) {
	this->size = size;
}

void awuiRectangle::Inflate(awuiSize size) {
	this->Inflate(size.GetWidth(), size.GetHeight());
}

void awuiRectangle::Inflate(int width, int height) {
	this->SetWidth(this->GetWidth() + width);
	this->SetHeight(this->GetHeight() + height);
}

void awuiRectangle::Offset(const awuiPoint pos) {
	this->Offset(pos.GetX(), pos.GetY());
}

void awuiRectangle::Offset(int x, int y) {
	this->SetX(this->GetX() + x);
	this->SetY(this->GetY() + y);
}

awuiRectangle & awuiRectangle::operator=(const awuiRectangle & other) {
	this->size = other.size;
	this->location = other.location;

	return *this;
}

awuiRectangle awuiRectangle::FromLTRB(int left, int top, int right, int bottom) {
 	return awuiRectangle(left, top, right - left + 1, bottom - top + 1);
}

awuiRectangle awuiRectangle::Intersect(const awuiRectangle rectangle1, const awuiRectangle rectangle2) {
	int left   = rectangle1.GetLeft() > rectangle2.GetLeft()?  rectangle1.GetLeft() : rectangle2.GetLeft();
	int top    = rectangle1.GetTop() > rectangle2.GetTop()? rectangle1.GetTop() : rectangle2.GetTop();
	int right  = rectangle1.GetRight() < rectangle2.GetRight()? rectangle1.GetRight() : rectangle2.GetRight();
	int bottom = rectangle1.GetBottom() < rectangle2.GetBottom()? rectangle1.GetBottom(): rectangle2.GetBottom();

	return awuiRectangle::FromLTRB(left, top, right, bottom);
}

void awuiRectangle::Intersect(const awuiRectangle rectangle) {
	*this = awuiRectangle::Intersect(*this, rectangle);
}
