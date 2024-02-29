// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Pen.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Drawing2D/LineCap.h>
#include <stdlib.h>

using namespace awui::Drawing;
using namespace awui::Drawing::Drawing2D;

Pen::Pen(Drawing::Color color) {
	m_color = color;
	m_width = 1;
	m_lineCap = LineCap::Butt;
	m_lineJoin = LineJoin::Miter;
}

Pen::Pen(Drawing::Color color, float width) {
	m_color = color;
	m_width = width;
	m_lineCap = LineCap::Butt;
	m_lineJoin = LineJoin::Miter;
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
	return m_color;
}

void Pen::SetColor(Drawing::Color color) {
	m_color = color;
}

float Pen::GetWidth() {
	return m_width;
}

void Pen::SetWidth(float width) {
	m_width = width;
}

void Pen::SetLineJoin(LineJoin lineJoin) {
	m_lineJoin = lineJoin;
}

LineJoin Pen::GetLineJoin() {
	return m_lineJoin;
}

void Pen::SetLineCap(LineCap lineCap) {
	m_lineCap = lineCap;
}

LineCap Pen::GetLineCap() {
	return m_lineCap;
}
