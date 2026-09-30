// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Pen.h"

#include <awui/Drawing/Color.h>
#include <stdlib.h>

using namespace awui;
using namespace awui::Drawing;

Pen::Pen(Color color) {
	m_color = color;
	m_width = 1;
	m_lineCap = LineCap::Butt;
	m_lineJoin = LineJoin::Miter;
}

Pen::Pen(Color color, float width) {
	m_color = color;
	m_width = width;
	m_lineCap = LineCap::Butt;
	m_lineJoin = LineJoin::Miter;
}

Color Pen::GetColor() const {
	return m_color;
}

void Pen::SetColor(Color color) {
	m_color = color;
}

float Pen::GetWidth() const {
	return m_width;
}

void Pen::SetWidth(float width) {
	m_width = width;
}

void Pen::SetLineJoin(LineJoin lineJoin) {
	m_lineJoin = lineJoin;
}

LineJoin Pen::GetLineJoin() const {
	return m_lineJoin;
}

void Pen::SetLineCap(LineCap lineCap) {
	m_lineCap = lineCap;
}

LineCap Pen::GetLineCap() const {
	return m_lineCap;
}
