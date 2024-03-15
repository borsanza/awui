/*
 * awui/awSlider/testWidget.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "SettingsWidget.h"

#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Bitmap.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Station;

SettingsWidget::SettingsWidget() {
	m_bitmap = new Bitmap("images/settings.png");
	SetFocusable(true);

	m_bitmap->SetLocation(0, 0);
	m_bitmap->SetSize(43, 46);
}

SettingsWidget::~SettingsWidget() {
	delete m_bitmap;
}

void SettingsWidget::OnPaint(GL *gl) {
	m_bitmap->OnPaint(gl);
}

awui::String SettingsWidget::ToString() const {
	return "awui::Windows::Forms::Station::SettingsWidget";
}