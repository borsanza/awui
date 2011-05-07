// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"
#include <awui/awuiGraphics.h>
#include <awui/awuiColor.h>
#include <awui/awuiPen.h>

extern "C" {
	#include <aw/sysgl.h>
}

Form1::Form1() {
	this->InitializeComponent();
}

Form1::~Form1() {
	delete this->pen1;
}

void Form1::InitializeComponent() {
	awuiColor * color3 = awuiColor::FromArgb(255, 255, 255);
	this->pen1 = new awuiPen(color3, 10);
	this->pen1->SetCap(awuiCap::Round);
	delete color3;
}

float Form1::GetAnimationValue() {
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

	return py;
}

void Form1::OnPaint(awuiGraphics * g) {
	float py = this->GetAnimationValue();

	awuiColor * color1 = awuiColor::FromArgb(0, (py * 255) / this->GetHeight(), 0);
	awuiColor * color2 = awuiColor::FromArgb((py * 255) / this->GetHeight(), 0, 0);

	g->FillRectangle(color1, 0, 0, this->GetWidth(), this->GetHeight());
	g->FillRectangle(color2, py, 100, this->GetWidth() - 100, this->GetHeight() - 100);

	delete color2;
	delete color1;

	g->DrawLine(this->pen1, 50, py, this->GetWidth() - 50, this->GetHeight() - py);
}