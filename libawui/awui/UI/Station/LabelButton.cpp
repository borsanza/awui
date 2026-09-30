/**
 * awui/UI/Station/LabelButton.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "LabelButton.h"

#include <awui/Math.h>
#include <awui/UI/Form.h>

using namespace awui::Drawing;
using namespace awui::UI::Station;
using namespace awui::UI::Events;

// Recorrido máximo (en píxeles, desde cualquiera de los dos extremos) con el que el texto vuelve deslizándose
static const float MaxSlideBack = 80.0f;

LabelButton::LabelButton() {
	m_time = 0.0f;
	m_lastSelected = false;
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetTextAlign(ContentAlignment::MiddleLeft);
}

void LabelButton::OnMouseUp(MouseEventArgs *e) {
	GetParent()->OnMouseUp(e);
}

void LabelButton::OnMouseDown(MouseEventArgs *e) {
	GetParent()->OnMouseDown(e);
}

void LabelButton::OnTick(float deltaSeconds) {
	Form *form = GetForm();
	if (!form) {
		return;
	}

	bool selected = (form->GetChildFocused() == GetParent());

	if (m_lastSelected != selected) {
		m_lastSelected = selected;
		m_time = 0.0f;
	}

	if (selected && (GetLabelWidth() > GetWidth())) {
		if (m_time < 2.0f) {
			SetScrolled(0.0f);
		} else {
			float prev = GetScrolled();
			SetScrolled(prev - (deltaSeconds * 120.0f));
			float after = GetScrolled();
			if (after > prev) {
				SetScrolled(0.0f);
				m_time = 0.0f;
			}
		}

		m_time += deltaSeconds;
	} else {
		float scrolled = GetScrolled();
		if (scrolled != 0) {
			// Al perder la selección el texto vuelve a su sitio por el camino más corto: hacia atrás al principio o,
			// pasada la mitad, hacia delante hasta la copia repetida (que visualmente es lo mismo)
			float dst = 0;
			float min = -(GetLabelWidth() + ScrollMargin);
			if ((GetLabelWidth() >> 1) < -scrolled)
				dst = min;

			dst = Math::Interpolate(scrolled, dst, deltaSeconds * 10.0f);
			if ((Math::Abs(dst) <= 1) || ((dst - 1) <= min))
				dst = 0;

			// Solo se desliza si le queda poco recorrido; si está lejos de los dos extremos salta al principio a
			// propósito (no se ve el texto entero pasando a toda velocidad)
			if ((dst <= -MaxSlideBack) && (dst >= (min + MaxSlideBack)))
				dst = 0;

			SetScrolled(dst);
		}

		m_time = 0.0f;
	}
}
