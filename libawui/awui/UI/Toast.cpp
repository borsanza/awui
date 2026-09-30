/**
 * awui/UI/Toast.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "Toast.h"

#include <awui/Drawing/Font.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/OpenGL/GL.h>
#include <awui/UI/TextRenderer.h>

#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::UI;

Toast::Toast() {
	m_image = nullptr;
	m_elapsed = -1.0f;
	m_opacity = 0.0f;
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetForeColor(Color::FromArgb(255, 255, 255));
	SetFont(Font("Liberation Sans", 24, FontStyle::Bold));
	SetDrawShadow(false);
	SetVisible(false);
}

Toast::~Toast() {
	delete m_image;
}

void Toast::Show(const String &text) {
	delete m_image;

	m_metrics = TextRenderer::GetMeasureText(text, GetFont());
	m_image = new Image(m_metrics.GetWidth(), m_metrics.GetHeight());
	Graphics *graphics = Graphics::FromImage(m_image);
	graphics->DrawString(text, GetFont(), GetForeColor(), 0, 0);
	delete graphics;

	SetSize(m_metrics.GetWidth() + (PaddingX * 2), m_metrics.GetHeight() + (PaddingY * 2));

	// Si ya se ve uno, el nuevo no vuelve a aparecer desde cero: se queda a la vista
	m_elapsed = (IsShowing() && (m_opacity > 0.0f)) ? (FadeInSeconds * m_opacity) : 0.0f;
	SetVisible(true);
}

void Toast::OnTick(float deltaSeconds) {
	if (!IsShowing())
		return;

	m_elapsed += deltaSeconds;

	if (m_elapsed < FadeInSeconds) {
		m_opacity = m_elapsed / FadeInSeconds;
	} else if (m_elapsed < FadeInSeconds + VisibleSeconds) {
		m_opacity = 1.0f;
	} else if (m_elapsed < FadeInSeconds + VisibleSeconds + FadeOutSeconds) {
		m_opacity = 1.0f - ((m_elapsed - FadeInSeconds - VisibleSeconds) / FadeOutSeconds);
	} else {
		m_opacity = 0.0f;
		m_elapsed = -1.0f;
		SetVisible(false);
	}
}

void Toast::OnPaint(GL *gl) {
	if (!m_image || (m_opacity <= 0.0f))
		return;

	// Fondo: negro semitransparente, más o menos según el fundido
	glColor4f(0.0f, 0.0f, 0.0f, 0.7f * m_opacity);
	GL::FillRectangle(0, 0, GetWidth() - 1, GetHeight() - 1);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	GL::DrawImageGL(m_image, PaddingX, PaddingY, m_image->GetWidth(), m_image->GetHeight(), m_opacity);
}
