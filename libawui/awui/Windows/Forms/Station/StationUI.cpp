/**
 * awui/Windows/Forms/Station/StationUI.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

 #include "StationUI.h"

#include <awui/Collections/SortedList.h>
#include <awui/Math.h>
#include <awui/Windows/Emulators/ArcadeContainer.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/ImageFader.h>
#include <awui/Windows/Forms/Station/Browser/Browser.h>
#include <awui/Windows/Forms/Station/Browser/Page.h>
#include <awui/Windows/Forms/Station/MenuButton.h>
#include <awui/Windows/Forms/Station/SettingsWidget.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

#define BORDERMARGIN 50
#define MENUBUTTONHEIGHT 70

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms::Station;
using namespace awui::Windows::Forms::Station::Browser;

StationUI::StationUI() {
	m_actual = 0;
	m_fade.SetStationUI(this);
	m_arcade = NULL;
	m_root = NULL;

	m_backgroundFader = new ImageFader();
	m_backgroundFader->SetTabStop(false);
	m_backgroundFader->SetDock(DockStyle::Fill);
	m_backgroundFader->SetColor(ColorF::FromArgb(0.25f, 1.0f, 1.0f, 1.0f));

	GetControls()->Add(m_backgroundFader);

	SetTabStop(false);

	Font font = Font("Liberation Sans", 38, FontStyle::Bold);
	Font font2 = Font("Liberation Sans", 22, FontStyle::Bold);
	Font fontClock = Font("Liberation Sans", 26, FontStyle::Bold);
	m_title = new Label();
	m_title->SetText("StationTV");
	m_title->SetTextAlign(ContentAlignment::BottomCenter);
	m_title->SetFont(font);
	m_title->SetDock(DockStyle::None);

	m_browser = new Browser::Browser();
	GetControls()->Add(m_title);
	GetControls()->Add(m_browser);

	m_settings = new SettingsWidget();
	m_settings->SetDock(DockStyle::None);
	m_settings->SetFont(font2);
	m_settings->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	m_settings->SetSize(44, 46);
	GetControls()->Add(m_settings);

	m_clock = new Label();
	m_clock->SetDock(DockStyle::None);
	m_clock->SetFont(fontClock);
	m_clock->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	m_clock->SetForeColor(Color::FromArgb(151, 151, 151));
	m_clock->SetTextAlign(ContentAlignment::TopCenter);
	m_clock->SetText("11:59");
	GetControls()->Add(m_clock);
}

StationUI::~StationUI() {
	// printf("~StationUI\n");
	if (m_root)
		delete m_root;
}

void StationUI::SetPath(const String path) {
	m_path = path;
}

void StationUI::Clear() {
	if (m_root) {
		delete m_root;
		m_root = 0;
	}
}

void StationUI::SetBackground(Bitmap * background) {
	m_backgroundFader->SetImage(background);
}

void StationUI::RecursiveSearch(NodeFile * parent) {
	DIR *d;
	struct dirent *dir;
	d = opendir(parent->m_path.ToCharArray());
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strcmp(dir->d_name, ".") == 0)
				continue;
			if (strcmp(dir->d_name, "..") == 0)
				continue;

			NodeFile * child = new NodeFile();

			String newFile = parent->m_path;

			if (!parent->m_path.EndsWith("/"))
				newFile += "/";

			newFile += dir->d_name;
			child->m_name = dir->d_name;
			child->m_background = NULL;

			child->m_button = new MenuButton(this);
			child->m_button->SetNodeFile(child);
			String name = child->m_name;
			name = name.Substring(0, name.LastIndexOf("."));
			child->m_button->SetText(name);

			if (parent->m_emulator == Types::Undefined) {
				if (child->m_name == "chip8") {
					child->m_emulator = Types::Chip8;
					child->m_button->SetText("CHIP-8");
					child->m_background = new Bitmap("./images/chip8.jpg");
					child->m_background->SetStretchMode(StretchMode::AspectFill);
				}

				if (child->m_name == "gamegear") {
					child->m_emulator = Types::GameGear;
					child->m_button->SetText("Game Gear");
					child->m_background = new Bitmap("./images/gamegear.jpg");
					child->m_background->SetStretchMode(StretchMode::AspectFill);
				}

				if (child->m_name == "mastersystem") {
					child->m_emulator = Types::MasterSystem;
					child->m_button->SetText("Master System");
					child->m_background = new Bitmap("./images/mastersystem.jpg");
					child->m_background->SetStretchMode(StretchMode::AspectFill);
				}

				if (child->m_name == "zxspectrum") {
					child->m_emulator = Types::Spectrum;
					child->m_button->SetText("ZX Spectrum");
					child->m_background = new Bitmap("./images/zxspectrum.jpg");
					child->m_background->SetStretchMode(StretchMode::AspectFill);
				}
			} else {
				child->m_emulator = parent->m_emulator;
			}

			if (!parent->m_childList)
				parent->m_childList = new SortedList();

			child->m_path = newFile;
			child->m_parent = parent;

			bool isDir = false;
			struct stat statbuf;
			if (stat(newFile.ToCharArray(), &statbuf) != -1)
				isDir = S_ISDIR(statbuf.st_mode);

			child->m_directory = isDir;

			child->m_key = String::Concat((child->m_directory? "1" : "2") , child->m_name);
			parent->m_childList->Add(&child->m_key, child);

			if (child->m_directory)
				RecursiveSearch(child);
		}

		closedir(d);
	}
}

bool StationUI::Minimize(NodeFile * parent) {
	int r = false;

	if (parent->m_childList) {
		for (int i = parent->m_childList->GetCount() - 1; i >= 0; i--) {
			NodeFile * child = (NodeFile *)parent->m_childList->GetByIndex(i);

			if (child->m_directory) {
				r |= Minimize(child);

				if (child->m_childList && child->m_childList->GetCount() > 0)
					continue;
			} else {
				switch (child->m_emulator) {
					case Types::Chip8:
						if (child->m_path.EndsWith("ch8")) continue;
						if (child->m_path.EndsWith("c8x")) continue;
						break;
					case Types::GameGear:
					case Types::MasterSystem:
						if (child->m_path.EndsWith("sms")) continue;
						if (child->m_path.EndsWith("sg")) continue;
						if (child->m_path.EndsWith("gg")) continue;
						break;
					case Types::Spectrum:
						if (child->m_path.EndsWith("rom")) continue;
						if (child->m_path.EndsWith("tap")) continue;
						break;
				}
			}

			delete child;
			parent->m_childList->RemoveAt(i);
			r = true;
		}
	}

	return r;
}

void StationUI::Refresh() {
	Clear();
	m_root = new NodeFile();
	m_actual = m_root;
	m_root->m_path = m_path;
	m_root->m_emulator = Types::Undefined;
	RecursiveSearch(m_root);
	while (Minimize(m_root));

	if (!m_root->m_childList) {
		m_root->m_childList = new SortedList();
		NodeFile * child = new NodeFile();
		child->m_name = "No hay roms";
		child->m_directory = false;
		child->m_button = new MenuButton(this);
		child->m_button->SetNodeFile(child);
		child->m_button->SetText(child->m_name);
		m_root->m_childList->Add(&child->m_name, child);
	}

	RefreshList();
}

void StationUI::RefreshList() {
	if (m_actual->m_page == NULL) {
		int y = 25;
		m_actual->m_page = new Page();
		for (int i = 0; i < m_actual->m_childList->GetCount(); i++) {
			NodeFile * child = (NodeFile *)m_actual->m_childList->GetByIndex(i);
			child->m_button->SetHeight(MENUBUTTONHEIGHT);
			child->m_button->SetLocation(40, y);
			y += MENUBUTTONHEIGHT;
			m_actual->m_page->GetControls()->Add(child->m_button);
			if (i == 0)
				child->m_button->SetFocus();
		}

		m_actual->m_page->SetHeight(y + 25);
	}

	m_browser->SetPage(m_actual->m_page);
}

void StationUI::OnTick(float deltaTime) {
	static Control * lastFocused = NULL;
	Control * c = m_actual->m_page->GetFocused();
	if (lastFocused != c) {
		lastFocused = c;
		CheckArcade();
	}

	time_t t;
	time(&t);
	struct tm *tm;
	tm = localtime(&t);
	char hora[6];
	strftime(hora, sizeof(hora), "%H:%M", tm);
	String horaS(hora);

	if (m_clock->GetText().CompareTo(horaS) != 0)
		m_clock->SetText(horaS);

	m_settings->SetLocation(GetWidth() - 150, 8);

	m_clock->SetLocation(GetWidth() - 80, 16);
	m_clock->SetSize(m_clock->GetLabelWidth(), 45);
	
	m_title->SetLocation(GetWidth() >> 1, 0);
	m_title->SetSize(GetWidth() >> 1, 69);
	m_browser->SetLocation(GetWidth() >> 1, 69);
	m_browser->SetSize(GetWidth() >> 1, GetHeight() - (69 + 25));
	m_actual->m_page->SetWidth(m_browser->GetWidth());

	for (int i = 0; i < m_actual->m_page->GetControls()->GetCount(); i++) {
		Control * child = (Control *)m_actual->m_page->GetControls()->Get(i);
		child->SetWidth(m_browser->GetWidth() - 100);
	}

	m_fade.SetBounds(0, 0, GetWidth(), GetHeight());

	if (m_arcade) {
		if (m_fade.IsFullScreen()) {
			m_arcade->SetBounds(0, 0, GetWidth(), GetHeight());
		} else {
			m_arcade->SetLocation(BORDERMARGIN, m_browser->GetTop());
			m_arcade->SetSize((GetWidth() >> 1) - BORDERMARGIN, m_browser->GetHeight());
		}
	}
}

void StationUI::SelectChild(NodeFile * node) {
	if (node->m_directory) {
		m_actual = node;
		RefreshList();
		m_actual->m_page->GetFocused()->SetFocus(true);
		UpdateTitle();
	} else {
		if (node->m_emulator != Types::Undefined) {
			if (!m_fade.IsStopped())
				return;
			if (GetControls()->IndexOf(&m_fade) == -1)
				GetControls()->Add(&m_fade);

			GetControls()->MoveToEnd(&m_fade);
			m_arcade->SetTabStop(true);
			m_arcade->SetFocus();
			m_fade.ShowFade();
		}
	}
}

void StationUI::SelectParent() {
	if (m_actual != NULL) {
		if (m_actual->m_parent != NULL) {
			m_actual = m_actual->m_parent;
			m_browser->SetPage(m_actual->m_page);
			m_actual->m_page->GetFocused()->SetFocus(true);
			UpdateTitle();
		}
	}
}

void StationUI::UpdateTitle() {
	if (m_actual == m_root)
		m_title->SetText("StationTV");
	else
		m_title->SetText(m_actual->m_button->GetText());
}

void StationUI::CheckArcade() {
	MenuButton * c = (MenuButton *) m_actual->m_page->GetFocused();
	if (c)
		c->CheckArcade();
}

void StationUI::SetArcade(Emulators::ArcadeContainer * arcade) {
	if (m_arcade == arcade)
		return;

	GetControls()->Replace(m_arcade, arcade);

	if (m_arcade) {
		m_arcade->SetSoundEnabled(false);
		m_arcade = NULL;
	}

	if (arcade) {
		m_arcade = arcade;
		m_arcade->SetSoundEnabled(true);
	}
}

void StationUI::SetArcadeFullScreen() {
	GetControls()->Remove(&m_fade);
	m_title->SetVisible(false);
	m_browser->SetVisible(false);
}

void StationUI::ExitingArcade() {
	if (!m_fade.IsStopped())
		return;

	m_title->SetVisible(true);
	m_browser->SetVisible(true);
	m_fade.HideFade();
	GetControls()->Add(&m_fade);
	m_arcade->SetTabStop(false);
	Control * c = m_actual->m_page->GetFocused();
	c->SetFocus(true);
}

void StationUI::ExitArcade() {
	GetControls()->Remove(&m_fade);
}

bool StationUI::OnKeyPress(Keys::Enum key) {
	if (m_arcade && (m_arcade->GetType() == Types::Chip8))
		return m_arcade->OnKeyPress(key);

	return Control::OnKeyPress(key);
}

bool StationUI::OnKeyUp(Keys::Enum key) {
	if (m_arcade && (m_arcade->GetType() == Types::Chip8))
		return m_arcade->OnKeyUp(key);

	return Control::OnKeyUp(key);
}

/********************************* FadePanel **********************************/

FadePanel::FadePanel() {
	m_station = NULL;
	m_showing = false;
	m_status = 0.0f;
}

FadePanel::~FadePanel() {
}

void FadePanel::ShowFade() {
	m_showing = true;
}

void FadePanel::HideFade() {
	m_showing = false;
}

void FadePanel::OnTick(float deltaTime) {
	if (m_showing) {
		m_status += 10;
		if (Math::Round(m_status) >= 200.0f) {
			m_status = 200.0f;
			m_station->SetArcadeFullScreen();
		}
	} else {
		m_status -= 10;
		if (Math::Round(m_status) <= 0.0f) {
			m_status = 0.0f;
			m_station->ExitArcade();
		}
	}

	if (m_status <= 100.0f)
		SetBackColor(Color::FromArgb(m_status * 2.55f, 0, 0, 0));
	else
		SetBackColor(Color::FromArgb((200.0f - m_status) * 2.55f, 0, 0, 0));
}

/********************************** NodeFile **********************************/

NodeFile::NodeFile() {
	m_parent = 0;
	m_childList = 0;
	m_directory = true;
	m_emulator = Types::Undefined;
	m_button = NULL;
	m_page = NULL;
	m_arcade = NULL;
}

NodeFile::~NodeFile() {
	// printf("%d) ~NodeFile:  %s\n", _emulator, _path.ToCharArray());
	if (m_childList) {
		for (int i = 0; i < m_childList->GetCount(); i++) {
			NodeFile * object = (NodeFile *)m_childList->GetByIndex(i);
			delete object;
		}

		delete m_childList;
	}

	if (m_button)
		delete m_button;
}
