/**
 * awui/UI/SelectionFrame.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "SelectionFrame.h"

#include <awui/Math.h>
#include <awui/OpenGL/Painter.h>
#include <awui/UI/Bitmap.h>
#include <awui/UI/Control.h>

#include <SDL_opengl.h>

using namespace awui;
using namespace awui::UI;

SelectionFrame::SelectionFrame() {
	m_parent = nullptr;
	m_left = 0.0f;
	m_top = 0.0f;
	m_right = 0.0f;
	m_bottom = 0.0f;
	m_valid = false;
	m_showing = false;
}

void SelectionFrame::OnTick(Control *focused, float deltaSeconds) {
	// Sin control que lo lleve no se pinta
	m_showing = focused && focused->GetDrawShadow() && focused->GetParent();
	if (!m_showing)
		return;

	const Control *parent = focused->GetParent();
	int x1, y1, x2, y2;
	Control::GetSelectedBitmap()->GetFixedMargins(&x1, &y1, &x2, &y2);
	float left = focused->GetLeft() - x1;
	float top = focused->GetTop() - y1;
	float right = focused->GetRight() + x2;
	float bottom = focused->GetBottom() + y2;

	if (!m_valid || (parent != m_parent)) {
		// Primera vez o en otro padre (otra página, el icono de ajustes...): aparece directamente en su sitio.
		// El cambio brusco es a propósito: marca que se ha cambiado de sitio
		m_valid = true;
		m_left = left;
		m_top = top;
		m_right = right;
		m_bottom = bottom;
	} else {
		float percent = Math::SmoothFactor(10.0f / 60.0f, deltaSeconds);
		m_left = Math::Interpolate(m_left, left, percent);
		m_top = Math::Interpolate(m_top, top, percent);
		m_right = Math::Interpolate(m_right, right, percent);
		m_bottom = Math::Interpolate(m_bottom, bottom, percent);
	}

	m_parent = parent;
}

void SelectionFrame::Paint(const Control *parent) {
	if (!m_showing || (parent != m_parent))
		return;

	Bitmap *bitmap = Control::GetSelectedBitmap();
	int left = Math::Round(m_left);
	int top = Math::Round(m_top);
	bitmap->SetSize(Math::Round(m_right - m_left + 1.0f), Math::Round(m_bottom - m_top + 1.0f));

	// El marco se pinta desplazado dentro del control que lo contiene
	OpenGL::Painter &painter = OpenGL::Painter::Instance();
	float offsetX = painter.GetOffsetX();
	float offsetY = painter.GetOffsetY();
	painter.SetOffset(offsetX + left, offsetY + top);
	bitmap->OnPaint(nullptr);
	painter.SetOffset(offsetX, offsetY);
}
