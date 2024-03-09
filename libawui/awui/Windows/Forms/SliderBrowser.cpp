/*
 * awui/Windows/Forms/SliderBrowser.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "SliderBrowser.h"

#include <awui/Console.h>
#include <awui/Effects/Effect.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>

using namespace awui::Effects;
using namespace awui::Windows::Forms;

SliderBrowser::SliderBrowser() {
	this->_margin = 8;
	this->_effect = new EffectLinear();
	this->_lastControl = NULL;
	this->_lastTime = 0;
	this->_initPos = 0;
	this->_selected = -1;
}

SliderBrowser::~SliderBrowser() {
	delete this->_effect;
}

void SliderBrowser::SetMargin(int margin) {
	this->_margin = margin;
}

void SliderBrowser::OnTick(float deltaSeconds) {
	int posSelected = this->GetControls()->IndexOf(Form::GetControlSelected());

	if (posSelected != -1)
		if (this->_selected != posSelected)
			this->_selected = posSelected;

	if (this->_selected == -1)
		this->_selected = 0;

	if (this->_selected >= this->GetControls()->GetCount())
		return;

	Control * w = (Control *)this->GetControls()->Get(this->_selected);

	bool leftOut = (w->GetLeft() - this->_margin) <= 0;
	bool rightOut = (w->GetRight() + this->_margin) >= this->GetWidth();
	if (leftOut || rightOut) {
		int left;
		if (leftOut)
			left = this->_margin;
		else
			left = this->GetWidth() - (w->GetWidth() + this->_margin);

		if (this->_lastControl != w) {
			this->_lastControl = w;
			this->_lastTime = 0;
			this->_initPos = w->GetLeft();
		}

		if (this->_lastTime < 10) {
			this->_lastTime++;
			float p = this->_effect->Calculate(this->_lastTime / 10.0f);
			left = this->_initPos + ((left - this->_initPos) * p);
		}

		w->SetLeft(left);
	}

	w->SetTop((this->GetHeight() - w->GetHeight()) >> 1);

	int x = w->GetLeft() + w->GetWidth() + this->_margin;
	for (int i = this->_selected + 1; i< this->GetControls()->GetCount(); i++) {
		x += this->_margin;
		Control * w2 = (Control *)this->GetControls()->Get(i);
		w2->SetLocation(x, (this->GetHeight() - w2->GetHeight()) >> 1);
		x += w2->GetWidth() + this->_margin;
	}

	x = w->GetLeft() - this->_margin;
	for (int i = this->_selected - 1; i >= 0; i--) {
		Control * w2 = (Control *)this->GetControls()->Get(i);
		x -= (this->_margin + w2->GetWidth());
		w2->SetLocation(x, (this->GetHeight() - w2->GetHeight()) >> 1);
		x -= this->_margin;
	}
}

Control * SliderBrowser::GetControlSelected() const {
	if ((this->_selected >= 0) && (this->_selected < this->GetControls()->GetCount()))
		return (Control *) this->GetControls()->Get(this->_selected);

	return NULL;
}
