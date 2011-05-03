// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

extern "C" {
	#include <aw/sysgl.h>
}

Form1::Form1() {
	this->InitializeComponent();
}

void Form1::InitializeComponent() {
}

void Form1::OnPaint() {
	static int sube = 1;
	static float py = (float) this->GetHeight();

	if (sube)
		py--;
	else
		py++;

	if (py < 0) {
		py = 0;
		sube = 0;
	}

	if (py > this->GetHeight()) {
		sube = 1;
		py = (float) this->GetHeight();
	}

	glColor3i(0, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0.0, py, 0.0);
	glVertex3f((float)this->GetWidth(), py, 0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(0, this->GetHeight() - py, 0);
	glVertex3f((float)this->GetWidth(), this->GetHeight() - py, 0);
	glEnd();
}