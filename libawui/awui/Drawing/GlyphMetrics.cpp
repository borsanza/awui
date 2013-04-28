// (c) Copyright 2011 Borja S�nchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "GlyphMetrics.h"

using namespace awui::Drawing;

GlyphMetrics::GlyphMetrics() {
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
