// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiRectangle.h"

#include "awuiPoint.h"
#include "awuiSize.h"

int awuiRectangle::GetWidth() {
	return this->size.GetWidth();
}

void awuiRectangle::SetWidth(int width) {
	this->size.SetWidth(width);
}

int awuiRectangle::GetHeight() {
	return this->size.GetHeight();
}

void awuiRectangle::SetHeight(int height) {
	this->size.SetHeight(height);
}

int awuiRectangle::GetX() {
	return this->location.GetX();
}

void awuiRectangle::SetX(int x) {
	this->location.SetX(x);
}

int awuiRectangle::GetY() {
	return this->location.GetY();
}

void awuiRectangle::SetY(int y) {
	this->location.SetY(y);
}

int awuiRectangle::GetBottom() {
	return this->GetY() + this->GetHeight() - 1;
}

int awuiRectangle::GetLeft() {
	return this->GetX();
}

int awuiRectangle::GetRight() {
	return this->GetX() + this->GetWidth() - 1;
}

int awuiRectangle::GetTop() {
	return this->GetY();
}

awuiPoint awuiRectangle::GetLocation() {
	return this->location;
}

awuiSize awuiRectangle::GetSize() {
	return this->size;
}

void awuiRectangle::SetLocation(awuiPoint location) {
	this->location = location;
}

void awuiRectangle::SetSize(awuiSize size) {
	this->size = size;
}

awuiRectangle & awuiRectangle::operator= (const awuiRectangle & other) {
	size = other.size;
	location = other.location;

	return *this;
}

void awuiRectangle::Intersect(awuiRectangle rectangle) {
	int rect1x1 = this->GetLeft();
	int rect2x1 = rectangle.GetLeft();
	int rect1x2 = this->GetRight();
	int rect2x2 = rectangle.GetRight();
	
	int rect1y1 = this->GetTop();
	int rect2y1 = rectangle.GetTop();
	int rect1y2 = this->GetBottom();
	int rect2y2 = rectangle.GetBottom();

	int x1 = rect1x1 > rect2x1? rect1x1: rect2x1;
	int y1 = rect1y1 > rect2y1? rect1y1: rect2y1;
	int x2 = rect1x2 < rect2x2? rect1x2: rect2x2;
	int y2 = rect1y2 < rect2y2? rect1y2: rect2y2;

	this->SetX(x1);
	this->SetY(y1);
	this->SetWidth(x2 - x1 + 1);
	this->SetHeight(y2 - y1 + 1);
}