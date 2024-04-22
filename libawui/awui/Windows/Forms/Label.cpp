/**
 * awui/Windows/Forms/Label.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Label.h"

#include <awui/Drawing/Font.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/TextRenderer.h>

#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

#define SCROLLMARGIN 80

Label::Label() {
	m_class = Classes::Label;
	m_scrolled = 0;
	m_image = NULL;
	m_g = NULL;
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetSize(75, 23);
	m_textAlign = ContentAlignment::TopLeft;
	SetForeColor(Color::FromArgb(255, 255, 255));
}

Label::~Label() {
	if (m_g)
		delete m_g;

	if (m_image)
		delete m_image;
}

bool Label::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Label) || Control::IsClass(objectClass);
}

int Label::GetLabelWidth() const {
	return m_metrics.GetAdvanceX() + m_metrics.GetBearingX();
}

#define BORDER 2

void Label::Draw(int x, int y, int width, int height) {
	if (m_image) {
		float posX = x;
		float posY = y;

		switch (m_textAlign) {
			case ContentAlignment::TopLeft:
			case ContentAlignment::TopCenter:
			case ContentAlignment::TopRight:
				posY += m_metrics.GetAscent();
				break;
			case ContentAlignment::MiddleLeft:
			case ContentAlignment::MiddleCenter:
			case ContentAlignment::MiddleRight:
				posY += m_metrics.GetAscent();
				posY += height - m_metrics.GetDescent() - 1;
				posY /= 2.0f;
				break;
			case ContentAlignment::BottomLeft:
			case ContentAlignment::BottomCenter:
			case ContentAlignment::BottomRight:
				posY += height - m_metrics.GetDescent() - 1;
				break;
		}

		switch (m_textAlign) {
			case ContentAlignment::TopLeft:
			case ContentAlignment::MiddleLeft:
			case ContentAlignment::BottomLeft:
				posX += m_metrics.GetBearingX();
				break;
			case ContentAlignment::TopCenter:
			case ContentAlignment::MiddleCenter:
			case ContentAlignment::BottomCenter:
				posX += width - GetLabelWidth() - 1;
				posX += m_metrics.GetBearingX();
				posX /= 2.0f;
				break;
			case ContentAlignment::TopRight:
			case ContentAlignment::MiddleRight:
			case ContentAlignment::BottomRight:
				posX += width - GetLabelWidth() - 1;
				break;
		}

		posX = Math::Round(posX);
		posY = Math::Round(posY);

		int correctPosX = posX - BORDER;
		int correctPosY = posY + m_metrics.GetBearingY() - BORDER;

		GL::DrawImageGL(m_image, correctPosX, correctPosY);
		//		DrawLines(posX, posY);
	}
}

void Label::OnPaint(GL *gl) {
	int scrolled = Math::Round(m_scrolled);
	Draw(scrolled, 0, GetWidth(), GetHeight());

	if (scrolled != 0)
		Draw(scrolled + (GetLabelWidth() + SCROLLMARGIN), 0, GetWidth(), GetHeight());
}

void Label::DrawLines(int x, int y) {
	int posBaseline = y;
	int posAscent = posBaseline - m_metrics.GetAscent();
	int posDescent = posBaseline + m_metrics.GetDescent();

	glColor3f(0.0f, 1.0f, 0.0f);
	GL::DrawLine(0, posBaseline, GetWidth(), posBaseline);

	glColor3f(1.0f, 0.0f, 1.0f);
	GL::DrawLine(0, posAscent, GetWidth(), posAscent);

	glColor3f(1.0f, 0.0f, 1.0f);
	GL::DrawLine(0, posDescent, GetWidth(), posDescent);

	glColor3f(1.0f, 0.0f, 0.0f);
	GL::DrawLine(x, 0, x, GetHeight());
	GL::DrawLine(0, y, GetWidth(), y);
}

void Label::SetText(const String str) {
	m_text = str;
	UpdateBufferText();
}

const awui::String Label::GetText() const {
	return m_text;
}

const awui::Drawing::ContentAlignment::Enum Label::GetTextAlign() {
	return m_textAlign;
}

void Label::SetTextAlign(Drawing::ContentAlignment::Enum textAlign) {
	m_textAlign = textAlign;
}

void Label::SetForeColor(const Color color) {
	Control::SetForeColor(color);
	UpdateBufferText();
}

void Label::SetFont(const Drawing::Font font) {
	Control::SetFont(font);
	UpdateBufferText();
}

void Label::UpdateBufferText() {
	if (m_g)
		delete m_g;

	if (m_image)
		delete m_image;

	Font *font = GetFont();
	m_metrics = TextRenderer::GetMeasureText(m_text, font);

	m_image = new Drawing::Image(m_metrics.GetWidth(), m_metrics.GetHeight());
	m_g = Drawing::Graphics::FromImage(m_image);

	m_g->DrawString(m_text, font, GetForeColor(), 0, 0);
}

void Label::SetScrolled(float scroll) {
	m_scrolled = scroll;
	while (m_scrolled <= -(GetLabelWidth() + SCROLLMARGIN))
		m_scrolled += GetLabelWidth() + SCROLLMARGIN;

	while (m_scrolled >= (GetLabelWidth() + SCROLLMARGIN))
		m_scrolled -= GetLabelWidth() + SCROLLMARGIN;
}

float Label::GetScrolled() const {
	return m_scrolled;
}
