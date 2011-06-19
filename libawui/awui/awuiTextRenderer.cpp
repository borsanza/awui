// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiTextRenderer.h"

#include "awuiGraphics.h"
#include "awuiImage.h"
#include "awuiSize.h"

awuiGraphics * awuiTextRenderer::graphics = NULL;
awuiImage * awuiTextRenderer::image = NULL;

awuiSize awuiTextRenderer::GetMeasureText(const std::string& text, awuiFont* font) {
	if (awuiTextRenderer::graphics == NULL) {
		awuiTextRenderer::image = new awuiImage(1, 1);
		awuiTextRenderer::graphics = awuiGraphics::FromImage(awuiTextRenderer::image);
	}

	return awuiTextRenderer::graphics->GetMeasureText(text, font);
}