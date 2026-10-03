/**
 * awui/UI/Emulators/KeyHelp.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "KeyHelp.h"

#include <awui/Drawing/GlyphMetrics.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/Drawing/Pen.h>
#include <awui/OpenGL/GL.h>
#include <awui/UI/TextRenderer.h>

#include <algorithm>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::UI::Emulators;

KeyHelp::KeyHelp() :
	m_titleFont("Liberation Sans", 30, FontStyle::Bold),
	m_sectionFont("Liberation Sans", 18, FontStyle::Bold),
	m_keyFont("Liberation Sans", 22, FontStyle::Bold),
	m_textFont("Liberation Sans", 22) {
	m_image = nullptr;
	m_showing = false;
	m_opacity = 0.0f;
	SetBackColor(Color::Transparent);
	SetDrawShadow(false);
	SetFocusable(false);
	SetVisible(false);
}

KeyHelp::~KeyHelp() {
	delete m_image;
}

void KeyHelp::Show(const String &title, const std::vector<Section> &sections) {
	// Medidas: la columna de las teclas es tan ancha como la tecla más larga
	GlyphMetrics titleSize = TextRenderer::GetMeasureText(title, &m_titleFont);
	int keysWidth = 0;
	int textWidth = 0;
	int rowHeight = 0;
	int sectionHeight = 0;
	for (const Section &section : sections) {
		GlyphMetrics size = TextRenderer::GetMeasureText(section.title, &m_sectionFont);
		textWidth = std::max(textWidth, size.GetWidth());
		sectionHeight = std::max(sectionHeight, size.GetHeight());
		for (const Row &row : section.rows) {
			GlyphMetrics keys = TextRenderer::GetMeasureText(row.keys, &m_keyFont);
			GlyphMetrics text = TextRenderer::GetMeasureText(row.description, &m_textFont);
			keysWidth = std::max(keysWidth, keys.GetWidth());
			textWidth = std::max(textWidth, text.GetWidth());
			rowHeight = std::max(rowHeight, std::max(keys.GetHeight(), text.GetHeight()));
		}
	}

	int contentWidth = std::max(titleSize.GetWidth(), keysWidth + ColumnGap + textWidth);
	int height = Padding + titleSize.GetHeight();
	for (const Section &section : sections)
		height += SectionGap + sectionHeight + RowGap + ((int) section.rows.size() * (rowHeight + RowGap));
	height += Padding;
	int width = Padding + contentWidth + Padding;

	delete m_image;
	m_image = new Image(width, height);
	Graphics *g = Graphics::FromImage(m_image);

	g->FillRoundedRectangle(Color::FromArgb(235, 24, 26, 32), 0, 0, width, height, Radius);
	Pen border(Color::FromArgb(60, 255, 255, 255), 1.5f);
	g->DrawRoundedRectangle(&border, 0.75f, 0.75f, width - 1.5f, height - 1.5f, Radius);

	int y = Padding;
	g->DrawString(title, &m_titleFont, Color::White, Padding, y);
	y += titleSize.GetHeight();

	Color accent = Color::FromArgb(255, 61, 165, 255);
	for (const Section &section : sections) {
		y += SectionGap;
		g->DrawString(section.title, &m_sectionFont, Color::FromArgb(255, 150, 155, 165), Padding, y);
		y += sectionHeight;
		Pen line(Color::FromArgb(50, 255, 255, 255), 1.0f);
		g->DrawLine(&line, Padding, y + (RowGap / 2) + 0.5f, width - Padding, y + (RowGap / 2) + 0.5f);
		y += RowGap;

		for (const Row &row : section.rows) {
			g->DrawString(row.keys, &m_keyFont, accent, Padding, y);
			g->DrawString(row.description, &m_textFont, Color::FromArgb(255, 230, 232, 236), Padding + keysWidth + ColumnGap, y);
			y += rowHeight + RowGap;
		}
	}
	delete g;

	m_showing = true;
	SetVisible(true);
}

void KeyHelp::Hide() {
	m_showing = false;
}

void KeyHelp::OnTick(float deltaSeconds) {
	// En el centro del padre (el juego), que puede cambiar de tamaño
	if (GetParent())
		SetBounds(0, 0, GetParent()->GetWidth(), GetParent()->GetHeight());

	float step = deltaSeconds / FadeSeconds;
	m_opacity = m_showing ? std::min(1.0f, m_opacity + step) : std::max(0.0f, m_opacity - step);
	if (!m_showing && (m_opacity <= 0.0f))
		SetVisible(false);
}

void KeyHelp::OnPaint(GL *gl) {
	if (!m_image || (m_opacity <= 0.0f))
		return;

	// Si no cabe (una ventana pequeña), se reduce entero
	float scale = std::min(1.0f, std::min((GetWidth() * 0.95f) / m_image->GetWidth(), (GetHeight() * 0.95f) / m_image->GetHeight()));
	int width = (int) (m_image->GetWidth() * scale);
	int height = (int) (m_image->GetHeight() * scale);
	GL::DrawImageGL(m_image, (GetWidth() - width) / 2, (GetHeight() - height) / 2, width, height, m_opacity);
}
