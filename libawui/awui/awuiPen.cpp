// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiPen.h"
#include "awuiColor.h"
#include <stdlib.h>

awuiPen::awuiPen(awuiColor * color) {
	this->color = NULL;
	this->SetColor(color);
	this->SetWidth(1);
	this->SetLineCap(awuiLineCap::Butt);
	this->SetLineJoin(awuiLineJoin::Miter);
}

awuiPen::awuiPen(awuiColor * color, float width) {
	this->color = NULL;
	this->SetColor(color);
	this->SetWidth(width);
	this->SetLineCap(awuiLineCap::Butt);
	this->SetLineJoin(awuiLineJoin::Miter);
}

awuiPen::~awuiPen() {
	delete this->color;
}

int awuiPen::IsClass(awuiObject::awuiClasses objectClass) {
	if (objectClass == awuiObject::Pen)
		return 1;

	return awuiObject::IsClass(objectClass);
}

awuiColor * awuiPen::GetColor() {
	return awuiColor::FromArgb(this->color->ToArgb());
}

void awuiPen::SetColor(awuiColor * color) {
	if (this->color != NULL)
		delete this->color;

	this->color = awuiColor::FromArgb(color->ToArgb());
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