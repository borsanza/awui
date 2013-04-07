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
#include <awui/animation/effects.h>

extern "C" {
	#include <aw/sysgl.h>
}

Form1::Form1() {
	this->InitializeComponent();
}

Form1::~Form1() {
}

void Form1::InitializeComponent() {
/*
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
	button->SetBackColor(awuiColor::FromArgb(0, 0, 255));
	button2->SetDock(awuiControl::Fill);
	button2->SetText("Button Top");
	button2->SetMinimumSize(awuiSize(75, 23));

	this->_splitter = new awuiSplitContainer();
	this->_splitter->SetName("SplitContainer");
	this->_splitter->GetPanel1()->GetControls()->Add(button);

	button = new awuiButton();
	button->SetDock(awuiControl::None);
	button->SetBounds(10, 10, 20, 20);
	button->SetText("Button Right2");
	button->SetBackColor(awuiColor::FromArgb(255, 0, 255));
	this->_splitter->GetPanel2()->GetControls()->Add(button2);
	this->_splitter->GetPanel2()->GetControls()->Add(button);
	this->_splitter->SetDock(awuiControl::Fill);
	this->_splitter->SetOrientation(awuiSplitContainer::Vertical);

	this->GetControls()->Add(this->_splitter);

    this->_panel = this->_splitter->GetPanel1();
*/
    this->_panel = new awuiPanel();
    this->_panel->SetDock(awuiControl::Fill);
    this->_panel->SetBackColor(awuiColor::FromArgb(0, 0, 0));
	this->GetControls()->Add(this->_panel);

    this->_buttons = new awuiArrayList();
    this->_effects = new awuiArrayList();

    this->AddButtonEffect(new EffectLinear());
    this->AddButtonEffect(new EffectSwing());
    this->AddButtonEffect(new EffectQuad());
    this->AddButtonEffect(new EffectCubic());
    this->AddButtonEffect(new EffectQuart());
    this->AddButtonEffect(new EffectQuint());
    this->AddButtonEffect(new EffectExpo());
    this->AddButtonEffect(new EffectSine());
    this->AddButtonEffect(new EffectCirc());
    this->AddButtonEffect(new EffectElastic());
    this->AddButtonEffect(new EffectBack());
    this->AddButtonEffect(new EffectBounce());

    this->SetSize(300, 910);
}

void Form1::AddButtonEffect(Effect * effect) {
    static int y = 5;

    for (int i = 0; i<=2; i++) {
        awuiButton * button = new awuiButton();
        button->SetDock(awuiControl::None);
        button->SetText(effect->GetName());
        button->SetSize(75, 18);
        button->SetTop(y);
        this->_panel->GetControls()->Add(button);

        this->_buttons->Add(button);
        this->_effects->Add(effect);

        y += button->GetHeight() + 5;
    }
}

void Form1::OnTick() {
    static float py = 0.0f;
    static int sube = 1;

    int time = this->_panel->GetWidth() / 3;

    py++;
    if (py > time) {
        py = 0.0f;
        sube = !sube;
    }

	float value = py / time;

    for (int i=0; i<this->_buttons->GetCount(); i++) {
        Effect * effect = (Effect *)this->_effects->Get(i);
        awuiButton * button = (awuiButton *)this->_buttons->Get(i);
        float value2;

        switch (i % 3) {
            case 0:
                EffectIn in;
                value2 = in.Calculate(value, effect);
                break;
            case 1:
                EffectOut out;
                value2 = out.Calculate(value, effect);
                break;
            case 2:
                EffectInOut inout;
                value2 = inout.Calculate(value, effect);
                break;
        }

        if (!sube)
            value2 = 1.0f - value2;

        int width = this->_panel->GetWidth() - button->GetWidth() - 10;
        button->SetLeft(width * value2 + 5);
    }
}
