// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "TextRenderer.h"

#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/Drawing/Size.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Graphics * TextRenderer::graphics = NULL;
Image * TextRenderer::image = NULL;

Size TextRenderer::GetMeasureText(const std::string& text, Font* font) {
	if (graphics == NULL) {
		image = new Drawing::Image(1, 1);
		graphics = Graphics::FromImage(image);
	}

	return graphics->GetMeasureText(text, font);
}
