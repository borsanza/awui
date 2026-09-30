// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "TextRenderer.h"

#include <awui/Drawing/GlyphMetrics.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/Drawing/Size.h>

using namespace awui::Drawing;
using namespace awui::UI;

Graphics *TextRenderer::s_graphics = NULL;
Image *TextRenderer::s_image = NULL;

GlyphMetrics TextRenderer::GetMeasureText(const String text, Font *font) {
	if (s_graphics == NULL) {
		s_image = new Drawing::Image(1, 1);
		s_graphics = Graphics::FromImage(s_image);
	}

	return s_graphics->GetMeasureText(text, font);
}
