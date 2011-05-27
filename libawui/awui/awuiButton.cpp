// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiButton.h"
#include "awuiColor.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
#include "awuiMouseEventArgs.h"
}

awuiButton::awuiButton() {
	this->SetSize(75,23);
	awuiColor * color = awuiColor::FromArgb(255, 255, 255);
	this->SetBackColor(color);
	delete color;

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
/*
float awuiButton::GetAnimationValue() {
	static int sube = 1;
	static float py = (float) 100.0f;

	if (sube)
		py--;
	else
		py++;

	if (py < 0) {
		py = 0;
		sube = 0;
	}

	if (py > 600.0f) {
		sube = 1;
		py = (float) 600.0f;
	}

	return py;
}
*/

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
	if (!this->show)
		return;

//	float py = this->GetAnimationValue();

	glColor3f(0.0f, 0.0f, 0.0f);
//	glBegin(GL_POLYGON);
//	glBegin(GL_LINE_LOOP);
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
}

void awuiButton::SetText(const std::string& str) {
	this->text.assign(str);
}

const std::string& awuiButton::GetName() {
	return this->text;
}