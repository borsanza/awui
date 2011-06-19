// (c) Copyright 2011 Borja S�nchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiSize.h"

#include "awuiPoint.h"

awuiSize::awuiSize() {
	this->SetWidth(0);
	this->SetHeight(0);
}

awuiSize::awuiSize(const awuiPoint pt) {
	this->SetWidth(pt.GetX());
	this->SetHeight(pt.GetY());
}

awuiSize::awuiSize(int width, int height) {
	this->SetWidth(width);
	this->SetHeight(height);
}

int awuiSize::GetWidth() const {
	return this->width;
}

void awuiSize::SetWidth(int width) {
	if (width < 0)
		width = 0;

	this->width = width;
}

int awuiSize::GetHeight() const {
	return this->height;
}

void awuiSize::SetHeight(int height) {
	if (height < 0)
		height = 0;

	this->height = height;
}

awuiSize & awuiSize::operator= (const awuiSize & other) {
	this->width = other.width;
	this->height = other.height;

	return *this;
}