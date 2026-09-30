// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "test2.h"

#include <awui/Console.h>
#include <awui/Drawing/Color.h>
#include <awui/Time/DateTime.h>
#include <awui/String.h>
#include <awui/Time/TimeSpan.h>
#include <awui/UI/Button.h>
#include <awui/UI/OnScreenKeyboard.h>
#include <awui/UI/ListBox.h>
#include <awui/UI/Diagnostics/Stats.h>

using namespace awui::Drawing;
using namespace awui::UI;
using namespace awui;

Test2::Test2() {
	m_listbox = NULL;
	m_runMame = true;
	m_endMame = false;
	InitializeComponent();
}

void Test2::InitializeComponent() {
	SetBackColor(Color::Transparent);
	SetDock(DockStyle::Left);
	SetSize(480, 400);

	OnScreenKeyboard *keyboard = new OnScreenKeyboard();
	keyboard->SetDock(DockStyle::Top);
	keyboard->SetSize(480, 480);
	AddWidget(keyboard);

	OnScreenKeyboard *keyboard2 = new OnScreenKeyboard();
	keyboard2->SetDock(DockStyle::Top);
	keyboard2->SetSize(480, 480);
	AddWidget(keyboard2);

	m_listbox = new ListBox();
	m_listbox->SetDock(DockStyle::Right);
	m_listbox->SetSize(480, 480);
	AddWidget(m_listbox);
}

void Test2::OnTick(float deltaSeconds) {
	if (!m_endMame) {
		CheckMame();
		// CheckGames();
	}
}

void Test2::CheckMame() {
	if (m_runMame) {
		m_runMame = false;
	}
}

/*
void Test2::CheckGames() {
	static int lines = 0;
	static awui::Time::TimeSpan lastTime;
	Diagnostics::Stats *stats = Diagnostics::Stats::Instance();

	awui::Time::TimeSpan time = stats->GetIdle();
	awui::Time::DateTime begin = awui::Time::DateTime::GetNow();
	awui::Time::DateTime end = begin;
	bool reRun = false;

	do {
		lines++;
		if (lines == 1)
			continue;

		end = awui::Time::DateTime::GetNow();

	} while ((end.GetTicks() - begin.GetTicks()) < ((time.GetTicks() - lastTime.GetTicks()) * 0.25));

	lastTime = end.GetTicks() - begin.GetTicks();

	if (reRun) {
		lines = 0;
		m_runMame = true;
		Console::WriteLine("-");
	}
}
*/