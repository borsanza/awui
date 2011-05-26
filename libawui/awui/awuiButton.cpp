// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiButton.h"
#include "awuiColor.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

awuiButton::awuiButton() {
	this->SetSize(75,23);
	awuiColor * color = awuiColor::FromArgb(255, 255, 255);
	this->SetBackColor(color);
	delete color;
}

awuiButton::~awuiButton() {
}

int awuiButton::IsClass(awuiObject::awuiClasses objectClass) {
	if (objectClass == awuiObject::Button)
		return 1;

	return awuiControl::IsClass(objectClass);
}

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

void awuiButton::OnPaint(awuiGL* gl) {
	float py = this->GetAnimationValue();

	glColor3f(0.0f, 0.0f, 0.0f);
	//glBegin(GL_POLYGON);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, py * this->GetHeight() / 600.0f);
	glVertex2f(this->GetWidth(), this->GetHeight() - (py * this->GetHeight() / 600.0f));
	glEnd();	
}