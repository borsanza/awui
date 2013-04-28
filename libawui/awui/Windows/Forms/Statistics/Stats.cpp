// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Stats.h"

#include <awui/Convert.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/GlyphMetrics.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <awui/TimeSpan.h>
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
	this->beforeSync_last = this->beforeSync;
	this->beforeSync = DateTime::GetNow();
}

void Stats::SetTimeAfterVSync() {
	this->afterSync_last = this->afterSync;
	this->afterSync = DateTime::GetNow();
}

void Stats::DrawString(String text, int x, int y) {
	Font font = Font("Monospace", 14);

	GlyphMetrics metrics = TextRenderer::GetMeasureText(text, &font);

	Drawing::Image * image = new Drawing::Image(metrics.GetWidth(), metrics.GetHeight());
	Drawing::Graphics * g = Drawing::Graphics::FromImage(image);
	g->DrawString(text, &font, Color::FromArgb(255, 255, 255), 0, 0);

	GL::DrawImageGL(image, x + metrics.GetBearingX(), y + metrics.GetBearingY());

	delete g;
	delete image;
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

	this->DrawString(letra, 5, 15);

	TimeSpan t(beforeSync.GetTicks() - afterSync.GetTicks());
	this->DrawString("Used:", 5, 35);
	this->DrawString(t.ToString(), 65, 35);

	TimeSpan t2(afterSync.GetTicks() - beforeSync_last.GetTicks());
	this->DrawString("Idle:", 5, 55);
	this->DrawString(t2.ToString(), 65, 55);

	TimeSpan t3(beforeSync.GetTicks() - beforeSync_last.GetTicks());
	this->DrawString("Total:", 5, 75);
	this->DrawString(t3.ToString(), 65, 75);

	pos = (pos + 1) % 4;
}
