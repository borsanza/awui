// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiButton.h"
#include "awuiColor.h"
#include "awuiMouseEventArgs.h"
#include "awuiTextRenderer.h"
#include "awuiFont.h"
#include "awuiImage.h"
#include "awuiGL.h"
#include "awuiGraphics.h"

#include <iostream>

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

awuiButton::awuiButton() {
	this->SetSize(75,23);
	this->SetBackColor(awuiColor::FromArgb(255, 255, 255));
	this->testx = 0;
	this->testy = 0;
	this->show = 0;
}

awuiButton::~awuiButton() {
}

int awuiButton::IsClass(awuiObject::awuiClasses objectClass) const {
	if (objectClass == awuiObject::Button)
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
	awuiFont font = awuiFont("Sans", 10);

	awuiSize size = awuiTextRenderer::GetMeasureText(this->text, &font);
	
	awuiImage * image = new awuiImage(size.GetWidth(), size.GetHeight());
	awuiGraphics * g = awuiGraphics::FromImage(image);
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