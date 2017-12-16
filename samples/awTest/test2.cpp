// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "test2.h"

#include <awui/DateTime.h>
#include <awui/Diagnostics/Process.h>
#include <awui/Drawing/Color.h>
#include <awui/String.h>
#include <awui/TimeSpan.h>
#include <awui/Console.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Keyboard.h>
#include <awui/Windows/Forms/ListBox.h>
#include <awui/Windows/Forms/Statistics/Stats.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Test2::Test2() {
	this->listbox = NULL;
	this->mameProcess = NULL;
	this->runMame = true;
	this->endMame = false;
	this->InitializeComponent();
}

Test2::~Test2() {
}

void Test2::InitializeComponent() {
	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->SetDock(DockStyle::Left);
	this->SetSize(480, 400);

	Keyboard * keyboard = new Keyboard();
	keyboard->SetDock(DockStyle::Top);
	keyboard->SetSize(480, 480);
	this->GetControls()->Add(keyboard);

	this->listbox = new ListBox();
	this->listbox->SetDock(DockStyle::Right);
	this->listbox->SetSize(480, 480);
	this->GetControls()->Add(this->listbox);
}

void Test2::OnTick() {
	if (!this->endMame) {
		this->CheckMame();
		this->CheckGames();
	}
}

void Test2::CheckMame() {
	if (this->runMame) {
		this->runMame = false;
		this->games.Clear();
		this->mameProcess = new awui::Diagnostics::Process();
		this->mameProcess->Start("mame", "-listfull");
	}
}

void Test2::CheckGames() {
	static int lines = 0;
	static awui::TimeSpan lastTime;
	Statistics::Stats * stats = Statistics::Stats::Instance();

	awui::TimeSpan time = stats->GetIdle();
	awui::DateTime begin = awui::DateTime::GetNow();
	awui::DateTime end = begin;
	bool reRun = false;

	do {
		if (this->mameProcess->GetHasExited()) {
//			this->endMame = true;
//			reRun = true;
			break;
		}

		if (!this->mameProcess->GetHasString())
			break;

		awui::String line = this->mameProcess->GetLine();
		lines++;
		if (lines == 1)
			continue;

		end = awui::DateTime::GetNow();

		int pos = line.IndexOf(" ");
		awui::String key = line.Substring(0, pos);

		pos = line.IndexOf("\"");
		awui::String name = line.Substring(pos + 1);
		pos = name.IndexOf("\"");
		name = name.Substring(0, pos);

		if (key.GetLength() > 0) {
			this->games.Add(&name, &key);
			this->listbox->GetItems()->Add(&name);
//			Console::WriteLine(key + ": " + name);
		}
	} while ((end.GetTicks() - begin.GetTicks()) < ((time.GetTicks() - lastTime.GetTicks()) * 0.25));

	lastTime = end.GetTicks() - begin.GetTicks();

//	Button * button = (Button *)this->_buttons->Get(0);
//	button->SetText(Convert::ToString(this->games.GetCount()) + " games");

	if (reRun) {
		delete this->mameProcess;
		lines = 0;
		this->mameProcess = NULL;
		this->runMame = true;
		Console::WriteLine("-");
	}
}
