// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Diagnostics/Process.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Pen.h>
#include <awui/Drawing/Size.h>
#include <awui/Effects/Effect.h>
#include <awui/Math.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Label.h>
#include <awui/Windows/Forms/Panel.h>
#include <awui/Windows/Forms/SplitContainer.h>
#include <awui/Windows/Forms/Statistics/Stats.h>

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
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));
	Bitmap * bitmap = new Bitmap("button.png");
	bitmap->SetDock(DockStyle::None);
	bitmap->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	bitmap->SetLocation(100, 100);
	bitmap->SetSize(Size(600, 150));
	bitmap->SetFixedMargins(22, 25, 22, 24);
	this->GetControls()->Add(bitmap);
/*
	Label * label;
	label = new Label();
	label->SetDock(DockStyle::Top);
	label->SetTextAlign(ContentAlignment::MiddleCenter);
	label->SetText("Label Top");
	label->SetMinimumSize(Size(75, 23));
	label->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	label->SetForeColor(Color::FromArgb(255, 255, 255));
	this->GetControls()->Add(label);

	label = new Label();
	label->SetDock(DockStyle::Bottom);
	label->SetTextAlign(ContentAlignment::MiddleCenter);
	label->SetText("Label Bottom");
	label->SetMinimumSize(Size(75, 23));
	label->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	label->SetForeColor(Color::FromArgb(255, 255, 255));
	this->GetControls()->Add(label);

	label = new Label();
	label->SetDock(DockStyle::Left);
	label->SetTextAlign(ContentAlignment::MiddleCenter);
	label->SetText("Label Left");
	label->SetMinimumSize(Size(75, 23));
	label->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	label->SetForeColor(Color::FromArgb(255, 255, 255));
	this->GetControls()->Add(label);

	label = new Label();
	label->SetDock(DockStyle::Right);
	label->SetTextAlign(ContentAlignment::MiddleCenter);
	label->SetText("Label Right");
	label->SetMinimumSize(Size(75, 23));
	label->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	label->SetForeColor(Color::FromArgb(255, 255, 255));
	this->GetControls()->Add(label);
*/
	this->_splitter = new SplitContainer();
	this->_splitter->SetName("SplitContainer");
	this->_splitter->SetDock(DockStyle::Fill);
	this->_splitter->SetOrientation(SplitContainer::Orientation::Vertical);
	this->_splitter->SetSplitterDistance(840);

	this->GetControls()->Add(this->_splitter);

	Control * control1 = this->_splitter->GetPanel1();
	Control * control2 = this->_splitter->GetPanel2();
	control1->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	control2->SetBackColor(Color::FromArgb(0, 0, 0, 0));

	this->_buttons = new awui::Collections::ArrayList();
	this->_effects = new awui::Collections::ArrayList();

	this->AddButtonEffect(new EffectLinear(), control1, 5);
	this->AddButtonEffect(new EffectSwing(), control1);
	this->AddButtonEffect(new EffectQuad(), control1);
	this->AddButtonEffect(new EffectCubic(), control1);
	this->AddButtonEffect(new EffectQuart(), control1);
	this->AddButtonEffect(new EffectQuint(), control1);

	this->AddButtonEffect(new EffectExpo(), control2, 5);
	this->AddButtonEffect(new EffectSine(), control2);
	this->AddButtonEffect(new EffectCirc(), control2);
	this->AddButtonEffect(new EffectElastic(), control2);
	this->AddButtonEffect(new EffectBack(), control2);
	this->AddButtonEffect(new EffectBounce(), control2);

	this->SetSize(300, 910);
	this->SetText("Test Form1");
//	this->SetFullscreen(0);
}

void Form1::AddButtonEffect(Effect * effect, Control * control, int posy) {
	static int y = 5;

	if (posy != -1)
		y = 5;

	for (int i = 0; i<=2; i++) {
		Button * button = new Button();
		button->SetDock(DockStyle::None);
		button->SetText(effect->GetName());
		button->SetSize(150, 52);
		button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
		button->SetForeColor(Color::FromArgb(255, 255, 255));
		button->SetFont(new Font("Monospace", 20, FontStyle::Bold)); // | FontStyle::Underline | FontStyle::Strikeout));
		button->SetTop(y);
		control->GetControls()->Add(button);

		this->_buttons->Add(button);
		this->_effects->Add(effect);

		y += button->GetHeight();
	}
}

void Form1::OnTick() {
	static float py1 = 0.0f;
	static float py2 = 0.0f;
	static int sube1 = 1;
	static int sube2 = 1;
	py1++;
	py2++;

	// Controles 1
	Control * control = ((Control *)this->_buttons->Get(0))->GetParent();
	int time = control->GetWidth() / 3;

	if (py1 > time) {
		py1 = 0.0f;
		sube1 = !sube1;
	}

	float value1 = py1 / time;

	control = ((Control *)this->_buttons->Get(18))->GetParent();
	time = control->GetWidth() / 3;

	if (py2 > time) {
		py2 = 0.0f;
		sube2 = !sube2;
	}

	float value2 = py2 / time;

	for (int i=0; i<this->_buttons->GetCount(); i++) {
		Effect * effect = (Effect *)this->_effects->Get(i);
		Button * button = (Button *)this->_buttons->Get(i);
		float value3;

		switch (i % 3) {
			case 0:
				EffectIn in;
				value3 = in.Calculate(i<18?value1:value2, effect);
				break;
			case 1:
				EffectOut out;
				value3 = out.Calculate(i<18?value1:value2, effect);
				break;
			case 2:
				EffectInOut inout;
				value3 = inout.Calculate(i<18?value1:value2, effect);
				break;
		}

		if (i < 18) {
			if (!sube1)
				value3 = 1.0f - value3;
		} else {
			if (!sube2)
				value3 = 1.0f - value3;
		}

		int width = button->GetParent()->GetWidth() - button->GetWidth() - 10;
		int left = awui::Math::Round(width * value3 + 5);
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
	static awui::TimeSpan lastTime;
	Statistics::Stats * stats = Statistics::Stats::Instance();

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
