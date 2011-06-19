// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"
#include <awui/awuiGraphics.h>
#include <awui/awuiColor.h>
#include <awui/awuiButton.h>
#include <awui/awuiSplitContainer.h>
#include <awui/awuiControlCollection.h>
#include <awui/awuiControl.h>
#include <awui/awuiPen.h>
#include <awui/awuiPanel.h>

extern "C" {
	#include <aw/sysgl.h>
}

Form1::Form1() {
	this->InitializeComponent();
}

Form1::~Form1() {
}

void Form1::InitializeComponent() {
	awuiButton * button;

	button = new awuiButton();
	button->SetDock(awuiControl::Top);
	button->SetText("Button Top");
	button->SetMinimumSize(awuiSize(75, 23));
	this->GetControls()->Add(button);

	button = new awuiButton();
	button->SetDock(awuiControl::Bottom);
	button->SetText("Button Bottom");
	button->SetMinimumSize(awuiSize(75, 23));
	this->GetControls()->Add(button);

	button = new awuiButton();
	button->SetDock(awuiControl::Left);
	button->SetText("Button Left");
	button->SetMinimumSize(awuiSize(75, 23));
	this->GetControls()->Add(button);

	button = new awuiButton();
	button->SetDock(awuiControl::Right);
	button->SetText("Button Right");
	button->SetMinimumSize(awuiSize(75, 23));
	this->GetControls()->Add(button);

	button = new awuiButton();
	awuiButton * button2 = new awuiButton();

	button->SetDock(awuiControl::Fill);
	button->SetText("Button Splitter Left");
	button->SetMinimumSize(awuiSize(75, 23));
	button2->SetDock(awuiControl::Fill);
	button2->SetText("Button Splitter Right");
	button2->SetMinimumSize(awuiSize(75, 23));
	
	this->splitter = new awuiSplitContainer();
	this->splitter->SetName("SplitContainer");
	this->splitter->GetPanel1()->GetControls()->Add(button);

	button = new awuiButton();
	button->SetDock(awuiControl::None);
	button->SetBounds(10, 10, 20, 20);
	button->SetText("Button Right2");
	button->SetBackColor(awuiColor::FromArgb(255, 0, 255));
	this->splitter->GetPanel2()->GetControls()->Add(button2);
	this->splitter->GetPanel2()->GetControls()->Add(button);
	this->splitter->SetDock(awuiControl::Fill);
	this->splitter->SetOrientation(awuiSplitContainer::Horizontal);

	this->GetControls()->Add(this->splitter);
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

void Form1::OnTick() {
	float py = this->GetAnimationValue();

//	this->splitter->SetSplitterDistance(this->splitter->GetWidth() * py / 100.0f);
}