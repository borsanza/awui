/**
 * awui/Windows/Forms/Station/LabelButton.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "LabelButton.h"

#include <awui/Windows/Forms/Form.h>
#include <awui/Math.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Station;

LabelButton::LabelButton() {
	m_time = 0.0f;
	m_lastSelected = false;
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetTextAlign(ContentAlignment::MiddleLeft);
	SetTabStop(false);
}

void LabelButton::OnMouseDown(MouseEventArgs * e) {
	GetParent()->OnMouseDown(e);
}

void LabelButton::OnTick(float deltaSeconds) {
	bool selected = (Form::GetControlSelected() == GetParent());

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
			float dst = 0;
			float min = -(GetLabelWidth() + 80);
			if ((GetLabelWidth() >> 1) < -scrolled)
				dst = min;

			dst = Math::Interpolate(scrolled, dst, deltaSeconds * 10.0f);
			if ((Math::Abs(dst) <= 1) || ((dst - 1) <= min))
				dst = 0;

			if ((dst <= -80) && (dst >= (min + 80)))
				dst = 0;

			SetScrolled(dst);
		}

		m_time = 0.0f;
	}
}
