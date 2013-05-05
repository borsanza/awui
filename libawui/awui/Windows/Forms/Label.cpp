// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Label.h"

#include <awui/Math.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <awui/Windows/Forms/TextRenderer.h>

#include <iostream>
#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Label::Label() {
	this->image = NULL;
	this->g = NULL;
	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->SetSize(75, 23);
	this->SetTextAlign(ContentAlignment::TopLeft);
	this->SetForeColor(Color::FromArgb(0, 0, 0));
}

Label::~Label() {
	if (this->g)
		delete this->g;

	if (this->image)
		delete this->image;
}

int Label::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Label)
		return 1;

	return Control::IsClass(objectClass);
}

#define BORDER 2

void Label::OnPaint(GL* gl) {
	if (image) {
		float posX = 0;
		float posY = 0;

		switch (this->textAlign) {
			case ContentAlignment::TopLeft:
			case ContentAlignment::TopCenter:
			case ContentAlignment::TopRight:
				posY = this->metrics.GetAscent();
				break;
			case ContentAlignment::MiddleLeft:
			case ContentAlignment::MiddleCenter:
			case ContentAlignment::MiddleRight:
				posY = this->metrics.GetAscent();
				posY += this->GetHeight() - this->metrics.GetDescent() - 1;
				posY /= 2.0f;
				break;
			case ContentAlignment::BottomLeft:
			case ContentAlignment::BottomCenter:
			case ContentAlignment::BottomRight:
				posY = this->GetHeight() - this->metrics.GetDescent() - 1;
				break;
		}

		switch (this->textAlign) {
			case ContentAlignment::TopLeft:
			case ContentAlignment::MiddleLeft:
			case ContentAlignment::BottomLeft:
				posX = this->metrics.GetBearingX();
				break;
			case ContentAlignment::TopCenter:
			case ContentAlignment::MiddleCenter:
			case ContentAlignment::BottomCenter:
				posX = this->GetWidth() - this->metrics.GetAdvanceX();
				posX += this->metrics.GetBearingX();
				posX /= 2.0f;
				break;
			case ContentAlignment::TopRight:
			case ContentAlignment::MiddleRight:
			case ContentAlignment::BottomRight:
				posX = this->GetWidth() - this->metrics.GetAdvanceX();
				break;
		}

		posX = Math::Round(posX);
		posY = Math::Round(posY);

		int correctPosX = posX - BORDER;
		int correctPosY = posY + this->metrics.GetBearingY() - BORDER;

		GL::DrawImageGL(image, correctPosX, correctPosY);
//		this->DrawLines(posX, posY);
	}
}

void Label::DrawLine(int x, int y, int x2, int y2) {
	glTranslatef(0.375, 0.375, 0);
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(x2, y2);
	glVertex2f(x2, y2);
	glVertex2f(x, y);
	glEnd();
	glTranslatef(-0.375, -0.375, 0);
}

void Label::DrawLines(int x, int y) {
	int posBaseline = y;
	int posAscent = posBaseline - this->metrics.GetAscent();
	int posDescent = posBaseline + this->metrics.GetDescent();

	glColor3f(0.0f, 1.0f, 0.0f);
	this->DrawLine(0, posBaseline, this->GetWidth(), posBaseline);

	glColor3f(1.0f, 0.0f, 1.0f);
	this->DrawLine(0, posAscent, this->GetWidth(), posAscent);

	glColor3f(1.0f, 0.0f, 1.0f);
	this->DrawLine(0, posDescent, this->GetWidth(), posDescent);

	glColor3f(1.0f, 0.0f, 0.0f);
	this->DrawLine(x, 0, x, this->GetHeight());
	this->DrawLine(0, y, this->GetWidth(), y);
}

void Label::SetText(const String str) {
	this->text = str;
	this->UpdateBufferText();
}

const awui::String Label::GetText() {
	return this->text;
}

const awui::Drawing::ContentAlignment::Enum Label::GetTextAlign() {
	return this->textAlign;
}

void Label::SetTextAlign(Drawing::ContentAlignment::Enum textAlign) {
	this->textAlign = textAlign;
}

void Label::SetForeColor(const Drawing::Color color) {
	Control::SetForeColor(color);
	this->UpdateBufferText();
}

void Label::SetFont(const Drawing::Font * font) {
	Control::SetFont(font);
	this->UpdateBufferText();
}

void Label::UpdateBufferText() {
	if (this->g)
		delete this->g;

	if (this->image)
		delete this->image;

	Font font = *this->GetFont();
	this->metrics = TextRenderer::GetMeasureText(this->text, &font);

	this->image = new Drawing::Image(this->metrics.GetWidth(), this->metrics.GetHeight());
	this->g = Drawing::Graphics::FromImage(this->image);
	this->g->DrawString(this->text, &font, this->GetForeColor(), 0, 0);
}
