// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Pen.h"

#include <awui/Drawing/Color.h>
#include <stdlib.h>

using namespace awui::Drawing;

Pen::Pen(Drawing::Color color) {
	this->SetColor(color);
	this->SetWidth(1);
	this->SetLineCap(awuiLineCap::Butt);
	this->SetLineJoin(awuiLineJoin::Miter);
}

Pen::Pen(Drawing::Color color, float width) {
	this->SetColor(color);
	this->SetWidth(width);
	this->SetLineCap(awuiLineCap::Butt);
	this->SetLineJoin(awuiLineJoin::Miter);
}

Pen::~Pen() {
}

int Pen::IsClass(Classes objectClass) const {
	if (objectClass == awui::Pen)
		return 1;

	return Object::IsClass(objectClass);
}

Color Pen::GetColor() {
	return this->color;
}

void Pen::SetColor(Drawing::Color color) {
	this->color = color;
}

float Pen::GetWidth() {
	return this->width;
}

void Pen::SetWidth(float width) {
	this->width = width;
}

void Pen::SetLineJoin(awuiLineJoin::LineJoin lineJoin) {
	this->lineJoin = lineJoin;
}

awuiLineJoin::LineJoin Pen::GetLineJoin() {
	return this->lineJoin;
}

void Pen::SetLineCap(awuiLineCap::LineCap lineCap) {
	this->lineCap = lineCap;
}

awuiLineCap::LineCap Pen::GetLineCap() {
	return this->lineCap;
}
