/*
 * samples/awTIS100/formTIS100.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formTIS100.h"

#include <awui/Drawing/Color.h>
#include <awui/Windows/Emulators/TIS100.h>

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms;

FormTIS100::FormTIS100() {
	InitializeComponent();
}

FormTIS100::~FormTIS100() {
}

void FormTIS100::InitializeComponent() {
	SetBackColor(Color::FromArgb(255, 8, 8, 8));

	SetSize(100, 100);
	SetFullscreen(0);
	SetText("awTIS100");
}

void FormTIS100::OnTick(float deltaSeconds) {
}
