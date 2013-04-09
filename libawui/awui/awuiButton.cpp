// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiButton.h"
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include "awuiMouseEventArgs.h"
#include "awuiTextRenderer.h"
#include "awuiGL.h"

#include <iostream>

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

awuiButton::awuiButton() {
	this->SetSize(75,23);
	this->SetBackColor(Color::FromArgb(255, 255, 255));
	this->testx = 0;
	this->testy = 0;
	this->show = 0;
}

awuiButton::~awuiButton() {
}

int awuiButton::IsClass(Classes objectClass) const {
	if (objectClass == awui::Button)
		return 1;

	return awuiControl::IsClass(objectClass);
}

void awuiButton::OnMouseLeave() {
	this->testx = -1;
	this->testy = -1;
	this->show = 0;
}

void awuiButton::OnMouseDown(awuiMouseEventArgs* e) {
	this->show = 1;
	this->testx = e->GetX();
	this->testy = e->GetY();
}

void awuiButton::OnMouseMove(awuiMouseEventArgs* e) {
	this->show = 1;
	this->testx = e->GetX();
	this->testy = e->GetY();
}

void awuiButton::OnPaint(awuiGL* gl) {
	Font font = Font("Sans", 12);

	Size size = awuiTextRenderer::GetMeasureText(this->text, &font);

	awui::Drawing::Image * image = new awui::Drawing::Image(size.GetWidth(), size.GetHeight());
	awui::Drawing::Graphics * g = awui::Drawing::Graphics::FromImage(image);
//	g->FillRectangle(awuiColor::FromArgb(250,0,0), 0, 0, size.GetWidth(), size.GetHeight());
//	g->Clear(awuiColor::FromArgb(255,0,0));
	// Esto hay que quitarlo, porque lo uso para definir un pen
	g->FillRectangle(Color::FromArgb(0,0,0), -10, -10, 1, 1);
	g->DrawString(this->text, &font, 0, size.GetHeight());

	awuiGL::DrawImageGL(image, (this->GetWidth() - size.GetWidth()) >> 1, (this->GetHeight() - size.GetHeight()) >> 1);

	delete g;
	delete image;

//	std::cout << size.GetWidth() << "x" << size.GetHeight() << ": " << this->text << std::endl;

	if (!this->show)
		return;

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(0, 0);
	glVertex2f(testx, this->testy);
	glVertex2f(this->GetWidth() - 1, 0);
	glVertex2f(testx, this->testy);
	glVertex2f(0, this->GetHeight() - 1);
	glVertex2f(testx, this->testy);
	glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
	glVertex2f(testx, this->testy);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(0, this->GetHeight() - 1);
	glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
	glVertex2f(this->GetWidth() - 1, 0);
	glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
	glVertex2f(0, 0);
	glVertex2f(this->GetWidth() - 1, 0);
	glVertex2f(0, 0);
	glVertex2f(0, this->GetHeight() - 1);
	glEnd();
}

void awuiButton::SetText(const std::string& str) {
	this->text.assign(str);
}

const std::string& awuiButton::GetName() {
	return this->text;
}
