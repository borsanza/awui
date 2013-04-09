// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiTextRenderer.h"

#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/Drawing/Size.h>

using namespace awui::Drawing;

Graphics * awuiTextRenderer::graphics = NULL;
Image * awuiTextRenderer::image = NULL;

Size awuiTextRenderer::GetMeasureText(const std::string& text, Font* font) {
	if (awuiTextRenderer::graphics == NULL) {
		awuiTextRenderer::image = new Image(1, 1);
		awuiTextRenderer::graphics = Graphics::FromImage(awuiTextRenderer::image);
	}

	return awuiTextRenderer::graphics->GetMeasureText(text, font);
}
