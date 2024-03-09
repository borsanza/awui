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
	SetSize(1, 1);
	m_image = new Drawing::Image(1, 1);
	SetSelectable(true);
}

TestWidget::~TestWidget() {
	delete m_image;
}

void TestWidget::OnTick(float deltaSeconds) {
//	printf("%d\n", GetParent()->GetControls()->IndexOf(this));
}

void TestWidget::OnPaint(GL* gl) {
	SetBackColor(Color::FromArgb(255, 255, 0, 0));
}