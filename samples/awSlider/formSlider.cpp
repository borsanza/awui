// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "formSlider.h"

#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Windows::Forms;

FormSlider::FormSlider() {
	this->InitializeComponent();
}

FormSlider::~FormSlider() {
}

void FormSlider::InitializeComponent() {
	this->SetText("Test FormSlider");

	this->SetSize(460, 460);
	this->SetFullscreen(0);
}
