// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Int32.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Pen.h>
#include <awui/Effects/Effect.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Panel.h>
#include <awui/Windows/Forms/SplitContainer.h>

extern "C" {
	#include <aw/sysgl.h>
}

using namespace awui::Collections;
using namespace awui::Drawing;
using namespace awui::Effects;
using namespace awui::Windows::Forms;

Form1::Form1() {
	this->InitializeComponent();
}

Form1::~Form1() {
}

void Form1::InitializeComponent() {
/*
	Button * button;

	button = new Button();
	button->SetDock(DockStyle::Top);
	button->SetText("Button Top");
	button->SetMinimumSize(Size(75, 23));
	this->GetControls()->Add(button);

	button = new Button();
	button->SetDock(DockStyle::Bottom);
	button->SetText("Button Bottom");
	button->SetMinimumSize(Size(75, 23));
	this->GetControls()->Add(button);

	button = new Button();
	button->SetDock(DockStyle::Left);
	button->SetText("Button Left");
	button->SetMinimumSize(Size(75, 23));
	this->GetControls()->Add(button);

	button = new Button();
	button->SetDock(DockStyle::Right);
	button->SetText("Button Right");
	button->SetMinimumSize(Size(75, 23));
	this->GetControls()->Add(button);

	button = new Button();
	Button * button2 = new Button();

	button->SetDock(DockStyle::Fill);
	button->SetText("Button Splitter Left");
	button->SetMinimumSize(Size(75, 23));
	button->SetBackColor(Color::FromArgb(0, 0, 255));
	button2->SetDock(DockStyle::Fill);
	button2->SetText("Button Top");
	button2->SetMinimumSize(Size(75, 23));

	this->_splitter = new SplitContainer();
	this->_splitter->SetName("SplitContainer");
	this->_splitter->GetPanel1()->GetControls()->Add(button);

	button = new Button();
	button->SetDock(DockStyle::None);
	button->SetBounds(10, 10, 20, 20);
	button->SetText("Button Right2");
	button->SetBackColor(Color::FromArgb(255, 0, 255));
	this->_splitter->GetPanel2()->GetControls()->Add(button2);
	this->_splitter->GetPanel2()->GetControls()->Add(button);
	this->_splitter->SetDock(DockStyle::Fill);
	this->_splitter->SetOrientation(SplitContainer::Vertical);

	this->GetControls()->Add(this->_splitter);

	this->_panel = this->_splitter->GetPanel1();
*/
	this->_panel = new Panel();
	this->_panel->SetDock(DockStyle::Fill);
	this->_panel->SetBackColor(Color::FromArgb(0, 0, 0));
	this->GetControls()->Add(this->_panel);

	this->_buttons = new awui::Collections::ArrayList();
	this->_effects = new awui::Collections::ArrayList();

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
	this->SetText("Test Form1");
}

void Form1::AddButtonEffect(Effect * effect) {
	static int y = 5;

	for (int i = 0; i<=2; i++) {
		Button * button = new Button();
		button->SetDock(DockStyle::None);
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
		Button * button = (Button *)this->_buttons->Get(i);
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
		awui::Int32 v = awui::Int32(width * value2 + 5);
		button->SetText(effect->GetName() + " " + &v);
	}
}
