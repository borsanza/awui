// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiPen.h"
#include "awuiColor.h"
#include <stdlib.h>

awuiPen::awuiPen(awuiColor color) {
	this->SetColor(color);
	this->SetWidth(1);
	this->SetLineCap(awuiLineCap::Butt);
	this->SetLineJoin(awuiLineJoin::Miter);
}

awuiPen::awuiPen(awuiColor color, float width) {
	this->SetColor(color);
	this->SetWidth(width);
	this->SetLineCap(awuiLineCap::Butt);
	this->SetLineJoin(awuiLineJoin::Miter);
}

awuiPen::~awuiPen() {
}

int awuiPen::IsClass(awuiObject::awuiClasses objectClass) const {
	if (objectClass == awuiObject::Pen)
		return 1;

	return awuiObject::IsClass(objectClass);
}

awuiColor awuiPen::GetColor() {
	return this->color;
}

void awuiPen::SetColor(awuiColor color) {
	this->color = color;
}

float awuiPen::GetWidth() {
	return this->width;
}

void awuiPen::SetWidth(float width) {
	this->width = width;
}

void awuiPen::SetLineJoin(awuiLineJoin::LineJoin lineJoin) {
	this->lineJoin = lineJoin;
}

awuiLineJoin::LineJoin awuiPen::GetLineJoin() {
	return this->lineJoin;
}

void awuiPen::SetLineCap(awuiLineCap::LineCap lineCap) {
	this->lineCap = lineCap;
}

awuiLineCap::LineCap awuiPen::GetLineCap() {
	return this->lineCap;
}
