/*
 * awui/Windows/Forms/SliderBrowser.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "SliderBrowser.h"

#include <awui/Console.h>
#include <awui/Effects/Effect.h>
#include <awui/Windows/Forms/Form.h>

using namespace awui::Effects;
using namespace awui::Windows::Forms;

SliderBrowser::SliderBrowser() {
	m_margin = 8;
	m_effect = new EffectLinear();
	m_lastControl = NULL;
	m_lastTime = 0;
	m_initPos = 0;
	m_selected = -1;
}

SliderBrowser::~SliderBrowser() {
	delete m_effect;
}

void SliderBrowser::SetMargin(int margin) {
	m_margin = margin;
}

void SliderBrowser::OnTick(float deltaSeconds) {
	int posSelected = IndexOf(Form::GetControlSelected());

	if (posSelected != -1)
		if (m_selected != posSelected)
			m_selected = posSelected;

	if (m_selected == -1)
		m_selected = 0;

	if (m_selected >= GetCount())
		return;

	Control * w = Get(m_selected);

	bool leftOut = (w->GetLeft() - m_margin) <= 0;
	bool rightOut = (w->GetRight() + m_margin) >= GetWidth();
	if (leftOut || rightOut) {
		int left;
		if (leftOut)
			left = m_margin;
		else
			left = GetWidth() - (w->GetWidth() + m_margin);

		if (m_lastControl != w) {
			m_lastControl = w;
			m_lastTime = 0;
			m_initPos = w->GetLeft();
		}

		if (m_lastTime < 10) {
			m_lastTime++;
			float p = m_effect->Calculate(m_lastTime / 10.0f);
			left = m_initPos + ((left - m_initPos) * p);
		}

		w->SetLeft(left);
	}

	w->SetTop((GetHeight() - w->GetHeight()) >> 1);

	int x = w->GetLeft() + w->GetWidth() + m_margin;
	for (int i = m_selected + 1; i< GetCount(); i++) {
		x += m_margin;
		Control * w2 = Get(i);
		w2->SetLocation(x, (GetHeight() - w2->GetHeight()) >> 1);
		x += w2->GetWidth() + m_margin;
	}

	x = w->GetLeft() - m_margin;
	for (int i = m_selected - 1; i >= 0; i--) {
		Control * w2 = Get(i);
		x -= (m_margin + w2->GetWidth());
		w2->SetLocation(x, (GetHeight() - w2->GetHeight()) >> 1);
		x -= m_margin;
	}
}

Control * SliderBrowser::GetControlSelected() const {
	if ((m_selected >= 0) && (m_selected < GetCount()))
		return Get(m_selected);

	return NULL;
}
