/*
 * awui/awSlider/formSlider.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formSlider.h"
#include "testWidget.h"

#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/SliderBrowser.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

FormSlider::FormSlider() {
	this->InitializeComponent();
}

FormSlider::~FormSlider() {
}

void FormSlider::InitializeComponent() {
	this->_slider = new SliderBrowser();
	this->_slider->SetDock(DockStyle::Fill);
	this->_slider->SetMargin(25);
	this->GetControls()->Add(_slider);

	for (int i = 0; i < 512; i++) {
		TestWidget * w = new TestWidget();
		w->SetSize(342, 262);
		w->SetLocation(25, 0);
		this->_slider->GetControls()->Add(w);
		w->SetTabStop(true);
	}

	this->SetText("Test FormSlider");
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));
	this->SetSize(1200, 850);
	this->SetFullscreen(0);
}