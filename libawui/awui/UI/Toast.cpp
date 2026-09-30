/**
 * awui/UI/Toast.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "Toast.h"

#include <awui/Drawing/Font.h>
#include <awui/Drawing/GlyphMetrics.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/Drawing/Pen.h>
#include <awui/Effects/Effect.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <awui/UI/TextRenderer.h>

using namespace awui::Drawing;
using namespace awui::Effects;
using namespace awui::OpenGL;
using namespace awui::UI;

Toast::Toast() {
	m_image = nullptr;
	m_elapsed = -1.0f;
	m_opacity = 0.0f;
	m_anchorLeft = 0;
	m_anchorBottom = 0;
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetForeColor(Color::FromArgb(255, 255, 255));
	SetFont(Font("Liberation Sans", 24, FontStyle::Bold));
	SetDrawShadow(false);
	SetScissorEnabled(false); // Al entrar y salir asoma por debajo del formulario
	SetVisible(false);
}

Toast::~Toast() {
	delete m_image;
}

void Toast::Show(const String &text) {
	delete m_image;

	GlyphMetrics metrics = TextRenderer::GetMeasureText(text, GetFont());
	int boxWidth = AccentWidth + PaddingX + metrics.GetWidth() + PaddingX;
	int boxHeight = PaddingY + metrics.GetHeight() + PaddingY;

	// Todo el recuadro se dibuja una vez con cairo (con antialiasing); al pintar solo se sube la imagen
	m_image = new Image(boxWidth + (Shadow * 2), boxHeight + (Shadow * 2));
	Graphics *g = Graphics::FromImage(m_image);

	// Sombra: capas cada vez más grandes y más tenues
	for (int i = Shadow; i > 0; i -= 2)
		g->FillRoundedRectangle(Color::FromArgb(18, 0, 0, 0), Shadow - i, Shadow - i + 3, boxWidth + (i * 2), boxHeight + (i * 2), Radius + i);

	g->FillRoundedRectangle(Color::FromArgb(235, 24, 26, 32), Shadow, Shadow, boxWidth, boxHeight, Radius);

	// Franja de color: el recuadro entero en ese color, recortado a su parte izquierda
	g->FillRoundedRectangle(Color::FromArgb(255, 61, 165, 255), Shadow, Shadow, AccentWidth + Radius, boxHeight, Radius);
	g->FillRectangle(Color::FromArgb(235, 24, 26, 32), Shadow + AccentWidth, Shadow, Radius, boxHeight);

	Pen border(Color::FromArgb(60, 255, 255, 255), 1.5f);
	g->DrawRoundedRectangle(&border, Shadow + 0.75f, Shadow + 0.75f, boxWidth - 1.5f, boxHeight - 1.5f, Radius);

	g->DrawString(text, GetFont(), GetForeColor(), Shadow + AccentWidth + PaddingX, Shadow + PaddingY);
	delete g;

	SetSize(m_image->GetWidth(), m_image->GetHeight());

	// Si ya se ve uno (entero o entrando), el nuevo ocupa su sitio sin volver a entrar desde fuera; si estaba
	// saliendo, vuelve a entrar desde donde estaba
	if (IsShowing()) {
		float outStart = InSeconds + VisibleSeconds;
		if (m_elapsed >= outStart)
			m_elapsed = InSeconds * (1.0f - ((m_elapsed - outStart) / OutSeconds));
		else if (m_elapsed > InSeconds)
			m_elapsed = InSeconds;
	} else {
		m_elapsed = 0.0f;
	}

	SetVisible(true);
	UpdatePosition(0.0f);
}

void Toast::SetAnchor(int left, int bottom) {
	m_anchorLeft = left;
	m_anchorBottom = bottom;
}

// offset: 0 en su sitio, 1 fuera de la pantalla por abajo
void Toast::UpdatePosition(float offset) {
	int restTop = m_anchorBottom - GetHeight() + Shadow;
	int hiddenTop = GetParent() ? GetParent()->GetHeight() : (restTop + GetHeight());
	SetLocation(m_anchorLeft - Shadow, Math::Round(restTop + ((hiddenTop - restTop) * offset)));
}

void Toast::OnTick(float deltaSeconds) {
	if (!IsShowing())
		return;

	m_elapsed += deltaSeconds;

	EffectCubic cubic;
	float offset;
	if (m_elapsed < InSeconds) {
		// Sube desde abajo frenando
		float p = m_elapsed / InSeconds;
		offset = 1.0f - EffectOut().Calculate(p, &cubic);
		m_opacity = p;
	} else if (m_elapsed < InSeconds + VisibleSeconds) {
		offset = 0.0f;
		m_opacity = 1.0f;
	} else if (m_elapsed < InSeconds + VisibleSeconds + OutSeconds) {
		// Baja acelerando
		float p = (m_elapsed - InSeconds - VisibleSeconds) / OutSeconds;
		offset = EffectIn().Calculate(p, &cubic);
		m_opacity = 1.0f - p;
	} else {
		m_opacity = 0.0f;
		m_elapsed = -1.0f;
		SetVisible(false);
		return;
	}

	UpdatePosition(offset);
}

void Toast::OnPaint(GL *gl) {
	if (!m_image || (m_opacity <= 0.0f))
		return;

	GL::DrawImageGL(m_image, 0, 0, m_image->GetWidth(), m_image->GetHeight(), m_opacity);
}
