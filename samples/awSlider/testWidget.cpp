/*
 * awui/awSlider/testWidget.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "testWidget.h"

#include <awui/Drawing/Image.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;

TestWidget::TestWidget() {
	this->SetSize(1, 1);
	this->_image = new Drawing::Image(1, 1);
	this->SetTabStop(true);
}

TestWidget::~TestWidget() {
	delete this->_image;
}

void TestWidget::OnTick(float deltaSeconds) {
//	printf("%d\n", this->GetParent()->GetControls()->IndexOf(this));
}

void TestWidget::OnPaint(GL* gl) {
	this->SetBackColor(Color::FromArgb(255, 255, 0, 0));
}