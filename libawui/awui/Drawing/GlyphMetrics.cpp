// (c) Copyright 2011 Borja Sanchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "GlyphMetrics.h"

using namespace awui::Drawing;

GlyphMetrics::GlyphMetrics() {
	this->width = 0;
	this->height = 0;
	this->ascent = 0;
	this->descent = 0;
	this->advancex = 0;
	this->advancey = 0;
	this->bearingx = 0;
	this->bearingy = 0;
}

int GlyphMetrics::GetWidth() const {
	return this->width;
}

int GlyphMetrics::GetHeight() const {
	return this->height;
}

void GlyphMetrics::SetWidth(int width) {
	if (width < 1)
		width = 1;

	this->width = width;
}

void GlyphMetrics::SetHeight(int height) {
	if (height < 1)
		height = 1;

	this->height = height;
}

int GlyphMetrics::GetAdvanceX() const {
	return this->advancex;
}

int GlyphMetrics::GetAdvanceY() const {
	return this->advancey;
}

void GlyphMetrics::SetAdvanceX(int advancex) {
	this->advancex = advancex;
}

void GlyphMetrics::SetAdvanceY(int advancey) {
	this->advancey = advancey;
}

int GlyphMetrics::GetBearingX() const {
	return this->bearingx;
}

int GlyphMetrics::GetBearingY() const {
	return this->bearingy;
}

void GlyphMetrics::SetBearingX(int bearingx) {
	this->bearingx = bearingx;
}

void GlyphMetrics::SetBearingY(int bearingy) {
	this->bearingy = bearingy;
}

int GlyphMetrics::GetAscent() const {
	return this->ascent;
}

int GlyphMetrics::GetDescent() const {
	return this->descent;
}

void GlyphMetrics::SetAscent(int ascent) {
	this->ascent = ascent;
}

void GlyphMetrics::SetDescent(int descent) {
	this->descent = descent;
}
