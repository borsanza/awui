// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"
#include <awui/awuiGraphics.h>
#include <awui/awuiColor.h>
#include <awui/awuiButton.h>
#include <awui/awuiPen.h>
#include <awui/awuiArrayList.h>

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
	awuiColor * color3 = awuiColor::FromArgb(0, 0, 0);
	this->pen1 = new awuiPen(color3, 5);
	this->pen1->SetCap(awuiCap::Round);
	delete color3;

	awuiButton * button = new awuiButton();
	button->SetLocation(50, 8);
	this->GetControls()->Add(button);
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

	g->DrawLine(this->pen1, 50.0f, py, this->GetWidth() - 50.0f, this->GetHeight() - py);

	awuiColor * color2 = awuiColor::FromArgb((int)((py * 255) / this->GetHeight()), (int)255 - ((py * 255) / this->GetHeight()), 0);
	g->FillRectangle(color2, py, 100.0f, this->GetWidth() - (py + 50.0f), this->GetHeight() - 150.0f);
	g->DrawRectangle(this->pen1, py, 100.0f, this->GetWidth() - (py + 50.0f), this->GetHeight() - 150.0f);

	delete color2;
}