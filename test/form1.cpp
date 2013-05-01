// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Diagnostics/Process.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Pen.h>
#include <awui/Effects/Effect.h>
#include <awui/Math.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Panel.h>
#include <awui/Windows/Forms/SplitContainer.h>

using namespace awui::Collections;
using namespace awui::Drawing;
using namespace awui::Effects;
using namespace awui::Windows::Forms;

Form1::Form1() {
	this->mameProcess = NULL;
	this->runMame = true;
	this->endMame = false;
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
	button->SetBackColor(Color::FromArgb(0, 0, 0));
	button2->SetDock(DockStyle::Fill);
	button2->SetText("Button Top");
	button2->SetMinimumSize(Size(75, 23));

	this->_splitter = new SplitContainer();
	this->_splitter->SetName("SplitContainer");
	this->_splitter->GetPanel1()->GetControls()->Add(button);

	button = new Button();
	button->SetDock(DockStyle::None);
	button->SetBounds(10, 10, 200, 30);
	button->SetText("Button Right2");
	button->SetBackColor(Color::FromArgb(0, 0, 0));
	this->_splitter->GetPanel2()->GetControls()->Add(button2);
	this->_splitter->GetPanel2()->GetControls()->Add(button);
	this->_splitter->SetDock(DockStyle::Fill);
	this->_splitter->SetOrientation(SplitContainer::Orientation::Vertical);

	this->GetControls()->Add(this->_splitter);

	this->_panel = this->_splitter->GetPanel1();
*/
	this->SetBackColor(Color::FromArgb(0, 0, 0));
	this->_panel = new Panel();
	this->_panel->SetDock(DockStyle::Fill);
	this->_panel->SetBackColor(Color::FromArgb(0, 0, 0, 0));
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
//	this->SetFullscreen(0);
}

void Form1::AddButtonEffect(Effect * effect) {
	static int y = 5;

	for (int i = 0; i<=2; i++) {
		Button * button = new Button();
		button->SetDock(DockStyle::None);
		button->SetText(effect->GetName());
		button->SetSize(150, 27);
		button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
		button->SetTop(y);
		this->_panel->GetControls()->Add(button);

		this->_buttons->Add(button);
		this->_effects->Add(effect);

		y += button->GetHeight();
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
		int left = awui::Math::Round(width * value2 + 5);
		button->SetLeft(left);
//		button->SetText(effect->GetName() + " " + awui::Convert::ToString(left));
	}

	if (!this->endMame) {
		this->CheckMame();
		this->CheckGames();
	}
}

void Form1::CheckMame() {
	if (this->runMame) {
		this->runMame = false;
		this->games.Clear();
		this->mameProcess = new Diagnostics::Process();
		this->mameProcess->Start("mame", "-listfull");
	}
}

void Form1::CheckGames() {
	static int lines = 0;
	static TimeSpan lastTime;
	Statistics::Stats * stats = this->GetStats();

	TimeSpan time = stats->GetIdle();
	DateTime begin = DateTime::GetNow();
	DateTime end = begin;
//	bool reRun = false;

	do {
		if (this->mameProcess->GetHasExited()) {
			this->endMame = true;
//			reRun = true;
			break;
		}

		if (!this->mameProcess->GetHasString())
			break;

		String line = this->mameProcess->GetLine();
		lines++;
		if (lines == 1)
			continue;

		end = DateTime::GetNow();

		int pos = line.IndexOf(" ");
		awui::String key = line.Substring(0, pos);

		pos = line.IndexOf("\"");
		awui::String name = line.Substring(pos + 1);
		pos = name.IndexOf("\"");
		name = name.Substring(0, pos);

		if (key.GetLength() > 0) {
			this->games.Add(&name, &key);
//			Console::WriteLine(key + ": " + name);
		}
	} while ((end.GetTicks() - begin.GetTicks()) < ((time.GetTicks() - lastTime.GetTicks()) * 0.25));

	lastTime = end.GetTicks() - begin.GetTicks();

	Button * button = (Button *)this->_buttons->Get(0);
	button->SetText(Convert::ToString(this->games.GetCount()) + " games");
/*
	if (reRun) {
		delete this->mameProcess;
		lines = 0;
		this->mameProcess = NULL;
		this->runMame = true;
	}
*/
}
