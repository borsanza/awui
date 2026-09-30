/*
 * awui/awSlider/formSlider.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formSlider.h"
#include "testWidget.h"

#include <awui/Drawing/Color.h>
#include <awui/UI/SliderBrowser.h>

using namespace awui::Drawing;
using namespace awui::UI;

FormSlider::FormSlider() {
	m_slider = NULL;
	InitializeComponent();
}

void FormSlider::InitializeComponent() {
	m_slider = new SliderBrowser();
	m_slider->SetDock(DockStyle::Fill);
	m_slider->SetMargin(25);
	AddWidget(m_slider);

	TestWidget *focused = nullptr;
	for (int i = 0; i < 512; i++) {
		TestWidget *w = new TestWidget();
		w->SetSize(342, 262);
		w->SetLocation(25, 0);
		m_slider->AddWidget(w);
		w->SetFocusable(true);

		if (!focused) {
			focused = w;
		}
	}

	if (focused) {
		focused->SetFocus();
	}

	SetText("Test FormSlider");
	SetBackColor(Color::FromArgb(255, 0, 0, 0));
	SetSize(1200, 850);
	SetFullscreen(0);
}
