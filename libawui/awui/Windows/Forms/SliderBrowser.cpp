/*
 * awui/Windows/Forms/SliderBrowser.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "SliderBrowser.h"

#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Windows::Forms;

SliderBrowser::SliderBrowser() {
	this->_margin = 8;
}

SliderBrowser::~SliderBrowser() {
}

void SliderBrowser::SetMargin(int margin) {
	this->_margin = margin;
}

void SliderBrowser::OnTick() {
	int pos = 0;
	for (int i = 0; i< this->GetControls()->GetCount(); i++) {
		pos += this->_margin;
		Control * w = (Control *)this->GetControls()->Get(i);
		w->SetLocation(pos, 0);
		pos += w->GetWidth() + this->_margin;
	}
}