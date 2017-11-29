/**
 * awui/Windows/Forms/Label.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Label.h"

#include <awui/Math.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/TextRenderer.h>

#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

#define SCROLLMARGIN 80

Label::Label() {
	this->_scrolled = 0;
	this->image = NULL;
	this->g = NULL;
	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->SetSize(75, 23);
	this->SetTextAlign(ContentAlignment::TopLeft);
	this->SetForeColor(Color::FromArgb(255, 255, 255));
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

int Label::GetLabelWidth() const {
	return this->metrics.GetAdvanceX() + this->metrics.GetBearingX();
}

#define BORDER 2

void Label::Draw(int x, int y, int width, int height) {
	if (this->image) {
		float posX = x;
		float posY = y;

		switch (this->textAlign) {
			case ContentAlignment::TopLeft:
			case ContentAlignment::TopCenter:
			case ContentAlignment::TopRight:
				posY += this->metrics.GetAscent();
				break;
			case ContentAlignment::MiddleLeft:
			case ContentAlignment::MiddleCenter:
			case ContentAlignment::MiddleRight:
				posY += this->metrics.GetAscent();
				posY += height - this->metrics.GetDescent() - 1;
				posY /= 2.0f;
				break;
			case ContentAlignment::BottomLeft:
			case ContentAlignment::BottomCenter:
			case ContentAlignment::BottomRight:
				posY += height - this->metrics.GetDescent() - 1;
				break;
		}

		switch (this->textAlign) {
			case ContentAlignment::TopLeft:
			case ContentAlignment::MiddleLeft:
			case ContentAlignment::BottomLeft:
				posX += this->metrics.GetBearingX();
				break;
			case ContentAlignment::TopCenter:
			case ContentAlignment::MiddleCenter:
			case ContentAlignment::BottomCenter:
				posX += width - this->GetLabelWidth() - 1;
				posX += this->metrics.GetBearingX();
				posX /= 2.0f;
				break;
			case ContentAlignment::TopRight:
			case ContentAlignment::MiddleRight:
			case ContentAlignment::BottomRight:
				posX += width - this->GetLabelWidth() - 1;
				break;
		}

		posX = Math::Round(posX);
		posY = Math::Round(posY);

		int correctPosX = posX - BORDER;
		int correctPosY = posY + this->metrics.GetBearingY() - BORDER;

		GL::DrawImageGL(this->image, correctPosX, correctPosY);
//		this->DrawLines(posX, posY);
	}
}

void Label::OnPaint(GL* gl) {
	int scrolled = Math::Round(this->_scrolled);
	this->Draw(scrolled, 0, this->GetWidth(), this->GetHeight());

	if (scrolled != 0)
		this->Draw(scrolled + (this->GetLabelWidth() + SCROLLMARGIN), 0, this->GetWidth(), this->GetHeight());
	else
		if (this->_scrolled)
			this->_scrolled = 0;
}

void Label::DrawLines(int x, int y) {
	int posBaseline = y;
	int posAscent = posBaseline - this->metrics.GetAscent();
	int posDescent = posBaseline + this->metrics.GetDescent();

	glColor3f(0.0f, 1.0f, 0.0f);
	GL::DrawLine(0, posBaseline, this->GetWidth(), posBaseline);

	glColor3f(1.0f, 0.0f, 1.0f);
	GL::DrawLine(0, posAscent, this->GetWidth(), posAscent);

	glColor3f(1.0f, 0.0f, 1.0f);
	GL::DrawLine(0, posDescent, this->GetWidth(), posDescent);

	glColor3f(1.0f, 0.0f, 0.0f);
	GL::DrawLine(x, 0, x, this->GetHeight());
	GL::DrawLine(0, y, this->GetWidth(), y);
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

void Label::SetFont(const Drawing::Font font) {
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

void Label::SetScrolled(float scroll) {
	this->_scrolled = scroll;
	while (this->_scrolled <= -(this->GetLabelWidth() + SCROLLMARGIN))
		this->_scrolled += this->GetLabelWidth() + SCROLLMARGIN;

	while (this->_scrolled >= (this->GetLabelWidth() + SCROLLMARGIN))
		this->_scrolled -= this->GetLabelWidth() + SCROLLMARGIN;
}

float Label::GetScrolled() const {
	return this->_scrolled;
}
