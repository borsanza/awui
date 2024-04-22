// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "test2.h"

#include <awui/Console.h>
#include <awui/Core/Color.h>
#include <awui/DateTime.h>
#include <awui/String.h>
#include <awui/TimeSpan.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/Keyboard.h>
#include <awui/Windows/Forms/ListBox.h>
#include <awui/Windows/Forms/Statistics/Stats.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Test2::Test2() {
	m_listbox = NULL;
	m_runMame = true;
	m_endMame = false;
	InitializeComponent();
}

void Test2::InitializeComponent() {
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetDock(DockStyle::Left);
	SetSize(480, 400);

	Keyboard *keyboard = new Keyboard();
	keyboard->SetDock(DockStyle::Top);
	keyboard->SetSize(480, 480);
	AddWidget(keyboard);

	Keyboard *keyboard2 = new Keyboard();
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
	static awui::TimeSpan lastTime;
	Statistics::Stats *stats = Statistics::Stats::Instance();

	awui::TimeSpan time = stats->GetIdle();
	awui::DateTime begin = awui::DateTime::GetNow();
	awui::DateTime end = begin;
	bool reRun = false;

	do {
		lines++;
		if (lines == 1)
			continue;

		end = awui::DateTime::GetNow();

	} while ((end.GetTicks() - begin.GetTicks()) < ((time.GetTicks() - lastTime.GetTicks()) * 0.25));

	lastTime = end.GetTicks() - begin.GetTicks();

	if (reRun) {
		lines = 0;
		m_runMame = true;
		Console::WriteLine("-");
	}
}
*/