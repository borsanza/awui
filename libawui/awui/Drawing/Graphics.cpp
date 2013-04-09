// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Graphics.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/Image.h>
#include <awui/Drawing/Pen.h>
#include <awui/Drawing/Size.h>

#include <cairo.h>
#include <stdlib.h>
#include <iostream>

using namespace awui::Drawing;

Graphics::Graphics() {
}

Graphics::~Graphics() {
}

int Graphics::IsClass(Classes objectClass) const {
	if (objectClass == awui::Graphics)
		return 1;

	return Object::IsClass(objectClass);
}

Graphics * Graphics::FromImage(Drawing::Image *image) {
	Graphics * graphics = new Graphics();
	graphics->cairo_surface = image->cairo_surface;
	graphics->cr = image->cr;

	return graphics;
}

void Graphics::DrawRectangle(Drawing::Pen * pen, float x, float y, float width, float height) {
	this->SetPen(pen);
	cairo_rectangle(this->cr, x, y, width, height);
	cairo_stroke(this->cr);
}

void Graphics::Clear(const Drawing::Color color) {
	cairo_set_source_rgba(this->cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	cairo_paint(this->cr);
}

void Graphics::FillRectangle(const Drawing::Color color, float x, float y, float width, float height) {
	cairo_set_source_rgba(this->cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	cairo_rectangle(this->cr, x, y, width, height);
	cairo_fill(this->cr);
}

void Graphics::DrawImage(Drawing::Image * image, float x, float y) {
	this->DrawImage(image, x, y, (float)image->GetWidth(), (float)image->GetHeight());
}

void Graphics::DrawImage(Drawing::Image * image, float x, float y, float width, float height) {
	cairo_surface_t *surfaceAux = image->cairo_surface;

	cairo_save(this->cr);
	cairo_translate(this->cr, x, y);
	cairo_scale(this->cr, image->GetWidth() / width, image->GetHeight() / height);

	cairo_set_source_surface(this->cr, surfaceAux, 0, 0);
	cairo_paint(this->cr);
	cairo_restore(this->cr);
}

void Graphics::SetPen(Drawing::Pen * pen) {
	Drawing::Color color = pen->GetColor();
	cairo_set_source_rgba(this->cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);

	cairo_set_line_width(this->cr, pen->GetWidth());

	switch (pen->GetLineCap()) {
		case awuiLineCap::Butt:
			cairo_set_line_cap(this->cr, CAIRO_LINE_CAP_BUTT);
			break;
		case awuiLineCap::Round:
			cairo_set_line_cap(this->cr, CAIRO_LINE_CAP_ROUND);
			break;
		case awuiLineCap::Square:
			cairo_set_line_cap(this->cr, CAIRO_LINE_CAP_SQUARE);
			break;
	}
}

void Graphics::DrawLine(Drawing::Pen * pen, float x1, float y1, float x2, float y2) {
	this->SetPen(pen);
	cairo_save(this->cr);
	cairo_move_to(this->cr, x1, y1);
	cairo_line_to(this->cr, x2, y2);
	cairo_stroke(this->cr);
	cairo_restore(this->cr);
}

Size Graphics::GetMeasureText(const std::string& text, Drawing::Font *font) const {
	cairo_text_extents_t extents;

	cairo_save(this->cr);
	cairo_select_font_face(this->cr, font->GetFont().c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(this->cr, font->GetSize());
	cairo_text_extents(this->cr, text.c_str(), &extents);
	cairo_restore(this->cr);

	return Size(extents.width + 4, extents.height + 4);
}

void Graphics::DrawString(const std::string& text, Drawing::Font * font, float x, float y) {
	cairo_text_extents_t extents;
	cairo_save(this->cr);
	cairo_select_font_face(this->cr, font->GetFont().c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(this->cr, font->GetSize());
	cairo_text_extents(this->cr, text.c_str(), &extents);
/*
	std::cout << "Bearing: " << extents.x_bearing << "x" << extents.y_bearing << std::endl;
	std::cout << "Size: " << extents.width << "x" << extents.height << std::endl;
	std::cout << "Advance: " << extents.x_advance << "x" << extents.y_advance << std::endl;
*/
	cairo_move_to(this->cr, x + 2 - extents.x_bearing, y - 2 - (extents.height + extents.y_bearing));
	cairo_show_text(this->cr, text.c_str());
	cairo_restore(this->cr);
}
