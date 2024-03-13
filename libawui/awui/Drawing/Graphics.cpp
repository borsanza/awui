// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Graphics.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/GlyphMetrics.h>
#include <awui/Drawing/Image.h>
#include <awui/Drawing/Pen.h>
#include <awui/Drawing/Size.h>
#include <awui/Math.h>

#include <cairo.h>

using namespace awui::Drawing;
using namespace awui::Drawing::Drawing2D;

Graphics::Graphics() {
	m_class = Classes::Graphics;
	m_cairo_surface = NULL;
	m_cr = NULL;
}

bool Graphics::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Graphics) || Object::IsClass(objectClass);
}

Graphics * Graphics::FromImage(Drawing::Image *image) {
	Graphics * graphics = new Graphics();
	graphics->m_cairo_surface = image->m_cairo_surface;
	graphics->m_cr = image->m_cr;

	return graphics;
}

void Graphics::DrawRectangle(Drawing::Pen * pen, float x, float y, float width, float height) {
	SetPen(pen);
	cairo_rectangle(m_cr, x, y, width, height);
	cairo_stroke(m_cr);
}

void Graphics::Clear(const Drawing::Color color) {
	cairo_set_source_rgba(m_cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	cairo_paint(m_cr);
}

void Graphics::FillRectangle(const Drawing::Color color, float x, float y, float width, float height) {
	cairo_set_source_rgba(m_cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	cairo_rectangle(m_cr, x, y, width, height);
	cairo_fill(m_cr);
}

void Graphics::DrawImage(Drawing::Image * image, float x, float y) {
	DrawImage(image, x, y, (float)image->GetWidth(), (float)image->GetHeight());
}

void Graphics::DrawImage(Drawing::Image * image, float x, float y, float width, float height) {
	cairo_surface_t *surfaceAux = image->m_cairo_surface;

	cairo_save(m_cr);
	cairo_translate(m_cr, x, y);
	cairo_scale(m_cr, image->GetWidth() / width, image->GetHeight() / height);

	cairo_set_source_surface(m_cr, surfaceAux, 0, 0);
	cairo_paint(m_cr);
	cairo_restore(m_cr);
}

void Graphics::SetPen(Drawing::Pen * pen) {
	Drawing::Color color = pen->GetColor();
	cairo_set_source_rgba(m_cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);

	cairo_set_line_width(m_cr, pen->GetWidth());

	switch (pen->GetLineCap()) {
		case LineCap::Butt:
			cairo_set_line_cap(m_cr, CAIRO_LINE_CAP_BUTT);
			break;
		case LineCap::Round:
			cairo_set_line_cap(m_cr, CAIRO_LINE_CAP_ROUND);
			break;
		case LineCap::Square:
			cairo_set_line_cap(m_cr, CAIRO_LINE_CAP_SQUARE);
			break;
	}
}

void Graphics::DrawLine(Drawing::Pen * pen, float x1, float y1, float x2, float y2) {
	SetPen(pen);
	cairo_save(m_cr);
	cairo_move_to(m_cr, x1, y1);
	cairo_line_to(m_cr, x2, y2);
	cairo_stroke(m_cr);
	cairo_restore(m_cr);
}

#define BORDER 2

GlyphMetrics Graphics::GetMeasureText(const String text, Drawing::Font *font) const {
	cairo_font_weight_t weight;
	cairo_font_slant_t slant;

	if (font->GetBold())
		weight = CAIRO_FONT_WEIGHT_BOLD;
	else
		weight = CAIRO_FONT_WEIGHT_NORMAL;

	if (font->GetItalic())
		slant = CAIRO_FONT_SLANT_ITALIC;
	else
		slant = CAIRO_FONT_SLANT_NORMAL;

	cairo_text_extents_t extents;
	cairo_font_extents_t fontExtents;

	cairo_save(m_cr);
	cairo_select_font_face(m_cr, font->GetFont().ToCharArray(), slant, weight);
	cairo_set_font_size(m_cr, font->GetSize());
	cairo_text_extents(m_cr, text.ToCharArray(), &extents);
	cairo_font_extents(m_cr, &fontExtents);
	cairo_restore(m_cr);

	GlyphMetrics metrics;
	metrics.SetWidth(extents.width + BORDER * 2);
	metrics.SetHeight(extents.height + BORDER * 2);
	metrics.SetAdvanceX(extents.x_advance);
	metrics.SetAdvanceY(extents.y_advance);
	metrics.SetBearingX(extents.x_bearing);
	metrics.SetBearingY(extents.y_bearing);

	metrics.SetAscent(fontExtents.ascent);
	metrics.SetDescent(fontExtents.descent);
	return metrics;
}

void Graphics::DrawString(const String text, Drawing::Font * font, const Drawing::Color color, float x, float y) {
	cairo_set_source_rgba(m_cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);

	cairo_font_weight_t weight;
	cairo_font_slant_t slant;

	if (font->GetBold())
		weight = CAIRO_FONT_WEIGHT_BOLD;
	else
		weight = CAIRO_FONT_WEIGHT_NORMAL;

	if (font->GetItalic())
		slant = CAIRO_FONT_SLANT_ITALIC;
	else
		slant = CAIRO_FONT_SLANT_NORMAL;

	cairo_text_extents_t extents;
	cairo_save(m_cr);
	cairo_select_font_face(m_cr, font->GetFont().ToCharArray(), slant, weight);
	cairo_set_font_size(m_cr, font->GetSize());
	cairo_text_extents(m_cr, text.ToCharArray(), &extents);

	int posx = x - extents.x_bearing + BORDER;
	int posy = y - extents.y_bearing + BORDER;
	cairo_move_to(m_cr, posx, posy);
	cairo_show_text(m_cr, text.ToCharArray());
	cairo_restore(m_cr);

	float size = font->GetSize() * 0.07f;
	size = Math::Max(Math::Round(size), 1.0f);
	Pen pen = Pen(color, size);

	cairo_antialias_t old = cairo_get_antialias(m_cr);
	cairo_set_antialias(m_cr, cairo_antialias_t::CAIRO_ANTIALIAS_NONE);

	if (font->GetStrikeout()) {
		float posy =  y - (extents.y_bearing / 2.0f) + BORDER;;
		posy = Math::Round(posy);
		DrawLine(&pen, BORDER, posy, extents.width + BORDER, posy);
	}

	if (font->GetUnderline()) {
		float posy =  y - extents.y_bearing + BORDER + (size * 1.5f);
		posy = Math::Round(posy);
		DrawLine(&pen, BORDER, posy, extents.width + BORDER, posy);
	}

	cairo_set_antialias(m_cr, old);
}
