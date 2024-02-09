// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Pen.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Drawing2D/LineCap.h>
#include <stdlib.h>

using namespace awui::Drawing;
using namespace awui::Drawing::Drawing2D;

Pen::Pen(Drawing::Color color) {
	this->color = color;
	this->width = 1;
	this->lineCap = LineCap::Butt;
	this->lineJoin = LineJoin::Miter;
}

Pen::Pen(Drawing::Color color, float width) {
	this->color = color;
	this->width = width;
	this->lineCap = LineCap::Butt;
	this->lineJoin = LineJoin::Miter;
}

Pen::~Pen() {
}

bool Pen::IsClass(Classes objectClass) const {
	if (objectClass == Classes::Pen) {
		return true;
	}

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

void Pen::SetLineJoin(LineJoin::Enum lineJoin) {
	this->lineJoin = lineJoin;
}

LineJoin::Enum Pen::GetLineJoin() {
	return this->lineJoin;
}

void Pen::SetLineCap(LineCap::Enum lineCap) {
	this->lineCap = lineCap;
}

LineCap::Enum Pen::GetLineCap() {
	return this->lineCap;
}
