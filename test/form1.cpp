// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"
#include <awui/awuiGraphics.h>
#include <awui/awuiColor.h>
#include <awui/awuiButton.h>
#include <awui/awuiSplitContainer.h>
#include <awui/awuiControl.h>
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

	awuiColor * color;
	awuiButton * button;
	
	button = new awuiButton();
	color = awuiColor::FromArgb(64, 64, 64);
	button->SetDock(awuiControl::Top);
	button->SetBackColor(color);
	delete color;
	this->GetControls()->Add(button);

	button = new awuiButton();
	color = awuiColor::FromArgb(128, 128, 128);
	button->SetDock(awuiControl::Bottom);
	button->SetBackColor(color);
	delete color;
	this->GetControls()->Add(button);

	button = new awuiButton();
	color = awuiColor::FromArgb(192, 192, 192);
	button->SetDock(awuiControl::Left);
	button->SetBackColor(color);
	delete color;
	this->GetControls()->Add(button);

	button = new awuiButton();
	color = awuiColor::FromArgb(255, 255, 255);
	button->SetDock(awuiControl::Right);
	button->SetBackColor(color);
	delete color;
	this->GetControls()->Add(button);

	color = awuiColor::FromArgb(255, 0, 0);
	this->SetBackColor(color);
	delete color;

	button = new awuiButton();
	awuiButton * button2 = new awuiButton();
	awuiSplitContainer * splitter = new awuiSplitContainer();
	splitter->GetControls()->Add(button);
	splitter->GetControls()->Add(button2);
	
	this->GetControls()->Add(splitter);
	splitter->SetDock(awuiControl::Fill);
}



float Form1::GetAnimationValue() {
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

	if (py > 100.0f) {
		sube = 1;
		py = (float) 100.0f;
	}

	return py;
}

void Form1::OnPaint(awuiGraphics * g) {
	float py = this->GetAnimationValue();

	g->DrawLine(this->pen1, 50.0f, py * this->GetHeight() / 100.0f, this->GetWidth() - 50.0f, this->GetHeight() - (py * this->GetHeight() / 100.0f));

	awuiColor * color2 = awuiColor::FromArgb((int)((py * 255.0f) / 100.0f), (int)(255.0f - ((py * 255.0f) / 100.0f)), 0);
	g->FillRectangle(color2, py * this->GetWidth() / 100.0f, 100.0f, this->GetWidth() - (py * this->GetWidth() / 100.0f + 50.0f), this->GetHeight() - 150.0f);
	g->DrawRectangle(this->pen1, py * this->GetWidth() / 100.0f, 100.0f, this->GetWidth() - (py * this->GetWidth() / 100.0f + 50.0f), this->GetHeight() - 150.0f);

	delete color2;
}
