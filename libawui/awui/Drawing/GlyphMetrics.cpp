// (c) Copyright 2011 Borja Sanchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "GlyphMetrics.h"

using namespace awui::Drawing;

GlyphMetrics::GlyphMetrics() {
	m_width = 0;
	m_height = 0;
	m_ascent = 0;
	m_descent = 0;
	m_advanceX = 0;
	m_advanceY = 0;
	m_bearingX = 0;
	m_bearingY = 0;
}

int GlyphMetrics::GetWidth() const {
	return m_width;
}

int GlyphMetrics::GetHeight() const {
	return m_height;
}

void GlyphMetrics::SetWidth(int width) {
	if (width < 1)
		width = 1;

	m_width = width;
}

void GlyphMetrics::SetHeight(int height) {
	if (height < 1)
		height = 1;

	m_height = height;
}

int GlyphMetrics::GetAdvanceX() const {
	return m_advanceX;
}

int GlyphMetrics::GetAdvanceY() const {
	return m_advanceY;
}

void GlyphMetrics::SetAdvanceX(int advancex) {
	m_advanceX = advancex;
}

void GlyphMetrics::SetAdvanceY(int advancey) {
	m_advanceY = advancey;
}

int GlyphMetrics::GetBearingX() const {
	return m_bearingX;
}

int GlyphMetrics::GetBearingY() const {
	return m_bearingY;
}

void GlyphMetrics::SetBearingX(int bearingx) {
	m_bearingX = bearingx;
}

void GlyphMetrics::SetBearingY(int bearingy) {
	m_bearingY = bearingy;
}

int GlyphMetrics::GetAscent() const {
	return m_ascent;
}

int GlyphMetrics::GetDescent() const {
	return m_descent;
}

void GlyphMetrics::SetAscent(int ascent) {
	m_ascent = ascent;
}

void GlyphMetrics::SetDescent(int descent) {
	m_descent = descent;
}
