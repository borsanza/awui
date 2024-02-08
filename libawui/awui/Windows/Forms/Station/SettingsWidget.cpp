/*
 * awui/awSlider/testWidget.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "SettingsWidget.h"

#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;

SettingsWidget::SettingsWidget() {
	this->_bitmap = new Bitmap("images/settings.png");
	this->SetTabStop(true);

	this->_bitmap->SetLocation(0, 0);
	this->_bitmap->SetSize(43, 46);
}

SettingsWidget::~SettingsWidget() {
	delete this->_bitmap;
}

void SettingsWidget::OnPaint(GL* gl) {
	this->_bitmap->OnPaint(gl);
}
