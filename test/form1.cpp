// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"
#include <awui/awuiGraphics.h>

extern "C" {
	#include <aw/sysgl.h>
}

Form1::Form1() {
	this->InitializeComponent();
}

void Form1::InitializeComponent() {
}

void Form1::OnPaint(awuiGraphics * g) {
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

	g->FillRectangle(0, (py * 255) / this->GetHeight(), 0, 255, 0, 0, this->GetWidth(), this->GetHeight());
	g->FillRectangle((py * 255) / this->GetHeight(), 0, 0, 255, py, 100, this->GetWidth() - 100, this->GetHeight() - 100);

	g->DrawLine(255, 255, 255, 255, 0, py, this->GetWidth(), this->GetHeight() - py);
}