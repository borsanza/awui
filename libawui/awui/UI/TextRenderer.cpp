// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "TextRenderer.h"

#include <awui/Drawing/GlyphMetrics.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/Drawing/Size.h>

using namespace awui::Drawing;
using namespace awui::UI;


static Graphics *GetGraphics() {
	static Image *image = new Image(1, 1);
	static Graphics *graphics = Graphics::FromImage(image);
	return graphics;
}

GlyphMetrics TextRenderer::GetMeasureText(const String text, Font *font) {
	return GetGraphics()->GetMeasureText(text, font);
}

std::vector<awui::String> TextRenderer::SplitLines(const String &text, Font *font, int width) {
	return GetGraphics()->SplitLines(text, font, width);
}
