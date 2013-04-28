// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Stats.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/GlyphMetrics.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/TextRenderer.h>
#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Statistics;

Stats::Stats() {
}

Stats::~Stats() {
}

void Stats::SetTimeBeforeVSync() {
}

void Stats::SetTimeAfterVSync() {
}

void Stats::Draw(GL* gl, int width, int height) {
	static int pos = 0;

	String letra;
	switch (pos) {
		case 0:
			letra = "─";
			break;
		case 1:
			letra = "╲";
			break;
		case 2:
			letra = "│";
			break;
		case 3:
			letra = "╱";
			break;
	}

	glDisable(GL_SCISSOR_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Font font = Font("Monospace", 15);

	GlyphMetrics metrics = TextRenderer::GetMeasureText(letra, &font);

	Drawing::Image * image = new Drawing::Image(metrics.GetWidth(), metrics.GetHeight());
	Drawing::Graphics * g = Drawing::Graphics::FromImage(image);
	g->DrawString(letra, &font, Color::FromArgb(255, 255, 255), 0, 0);

	GL::DrawImageGL(image, 5 + metrics.GetBearingX(), 15 + metrics.GetBearingY());

	delete g;
	delete image;

	pos = (pos + 1) % 4;
}
