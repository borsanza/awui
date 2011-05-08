// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiPen.h"
#include "awuiColor.h"
#include <stdlib.h>

awuiPen::awuiPen(awuiColor * color) {
	this->color = NULL;
	this->SetColor(color);
	this->SetWidth(1);
	this->SetCap(awuiCap::Butt);
	this->SetLineJoin(awuiLineJoin::Miter);
}

awuiPen::awuiPen(awuiColor * color, float width) {
	this->color = NULL;
	this->SetColor(color);
	this->SetWidth(width);
	this->SetCap(awuiCap::Butt);
	this->SetLineJoin(awuiLineJoin::Miter);
}

awuiPen::~awuiPen() {
	delete this->color;
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

void awuiPen::SetCap(awuiCap::Cap cap) {
	this->cap = cap;
}

awuiCap::Cap awuiPen::GetCap() {
	return this->cap;
}