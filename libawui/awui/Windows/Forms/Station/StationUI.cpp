/**
 * awui/Windows/Forms/Station/StationUI.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

 #include "StationUI.h"

#include <awui/Math.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Station/Browser/Browser.h>
#include <awui/Windows/Forms/Station/Browser/Page.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define BORDERMARGIN 50
#define MENUBUTTONHEIGHT 70

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms::Station;
using namespace awui::Windows::Forms::Station::Browser;

StationUI::StationUI() {
	this->_actual = 0;
	this->_fade.SetStationUI(this);
	this->_arcade = NULL;
	this->_root = NULL;

	this->SetTabStop(false);

	Font font = Font("Liberation Sans", 38, FontStyle::Bold);
	Font font2 = Font("Liberation Sans", 22, FontStyle::Bold);
	this->_title = new Label();
	this->_title->SetText("StationTV");
	this->_title->SetTextAlign(ContentAlignment::BottomCenter);
	this->_title->SetFont(font);
	this->_title->SetDock(DockStyle::None);

	this->_browser = new Browser::Browser();
	this->GetControls()->Add(this->_title);
	this->GetControls()->Add(this->_browser);

	this->_settings = new Button();
	this->_settings->SetDock(DockStyle::None);
	this->_settings->SetFont(font2);
	this->_settings->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->_settings->SetText("Settings");
	this->GetControls()->Add(this->_settings);
}

StationUI::~StationUI() {
	// printf("~StationUI\n");
	if (this->_root)
		delete this->_root;
}

void StationUI::SetPath(const String path) {
	this->_path = path;
}

void StationUI::Clear() {
	if (this->_root) {
		delete this->_root;
		this->_root = 0;
	}
}

void StationUI::RecursiveSearch(NodeFile * parent) {
	DIR *d;
	struct dirent *dir;
	d = opendir(parent->_path.ToCharArray());
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strcmp(dir->d_name, ".") == 0)
				continue;
			if (strcmp(dir->d_name, "..") == 0)
				continue;

			NodeFile * child = new NodeFile();

			String newFile = parent->_path;

			if (!parent->_path.EndsWith("/"))
				newFile += "/";

			newFile += dir->d_name;
			child->_name = dir->d_name;

			child->_button = new MenuButton(this);
			child->_button->SetNodeFile(child);
			String name = child->_name;
			name = name.Substring(0, name.LastIndexOf("."));
			child->_button->SetText(name);

			if (parent->_emulator == Types::Undefined) {
				if (child->_name == "chip8") {
					child->_emulator = Types::Chip8;
					child->_button->SetText("CHIP-8");
				}

				if (child->_name == "gamegear") {
					child->_emulator = Types::GameGear;
					child->_button->SetText("Game Gear");
				}

				if (child->_name == "mastersystem") {
					child->_emulator = Types::MasterSystem;
					child->_button->SetText("Master System");
				}

				if (child->_name == "zxspectrum") {
					child->_emulator = Types::Spectrum;
					child->_button->SetText("ZX Spectrum");
				}
			} else {
				child->_emulator = parent->_emulator;
			}

			if (!parent->_childList)
				parent->_childList = new SortedList();

			child->_path = newFile;
			child->_parent = parent;

			bool isDir = false;
			struct stat statbuf;
			if (stat(newFile.ToCharArray(), &statbuf) != -1)
				isDir = S_ISDIR(statbuf.st_mode);

			child->_directory = isDir;

			child->_key = String::Concat((child->_directory? "1" : "2") , child->_name);
			parent->_childList->Add(&child->_key, child);

			if (child->_directory)
				this->RecursiveSearch(child);
		}

		closedir(d);
	}
}

bool StationUI::Minimize(NodeFile * parent) {
	int r = false;

	if (parent->_childList) {
		for (int i = parent->_childList->GetCount() - 1; i >= 0; i--) {
			NodeFile * child = (NodeFile *)parent->_childList->GetByIndex(i);

			if (child->_directory) {
				r |= Minimize(child);

				if (child->_childList && child->_childList->GetCount() > 0)
					continue;
			} else {
				switch (child->_emulator) {
					case Types::Chip8:
						if (child->_path.EndsWith("ch8")) continue;
						if (child->_path.EndsWith("c8x")) continue;
						break;
					case Types::GameGear:
					case Types::MasterSystem:
						if (child->_path.EndsWith("sms")) continue;
						if (child->_path.EndsWith("sg")) continue;
						break;
					case Types::Spectrum:
						if (child->_path.EndsWith("rom")) continue;
						if (child->_path.EndsWith("tap")) continue;
						break;
				}
			}

			delete child;
			parent->_childList->RemoveAt(i);
			r = true;
		}
	}

	return r;
}

void StationUI::Refresh() {
	this->Clear();
	this->_root = new NodeFile();
	this->_actual = this->_root;
	this->_root->_path = this->_path;
	this->_root->_emulator = Types::Undefined;
	this->RecursiveSearch(this->_root);
	while (this->Minimize(this->_root));

	if (!this->_root->_childList) {
		this->_root->_childList = new SortedList();
		NodeFile * child = new NodeFile();
		child->_name = "No hay roms";
		child->_directory = false;
		child->_button = new MenuButton(this);
		child->_button->SetNodeFile(child);
		child->_button->SetText(child->_name);
		this->_root->_childList->Add(&child->_name, child);
	}

	this->RefreshList();
}

void StationUI::RefreshList() {
	if (this->_actual->_page == NULL) {
		int y = 25;
		this->_actual->_page = new Page();
		for (int i = 0; i < this->_actual->_childList->GetCount(); i++) {
			NodeFile * child = (NodeFile *)this->_actual->_childList->GetByIndex(i);
			child->_button->SetHeight(MENUBUTTONHEIGHT);
			child->_button->SetLocation(40, y);
			y += MENUBUTTONHEIGHT;
			this->_actual->_page->GetControls()->Add(child->_button);
			if (i == 0)
				child->_button->SetFocus();
		}

		this->_actual->_page->SetHeight(y + 25);
	}

	this->_browser->SetPage(this->_actual->_page);
}

void StationUI::OnTick() {
	static Control * lastFocused = NULL;
	Control * c = this->_actual->_page->GetFocused();
	if (lastFocused != c) {
		lastFocused = c;
		this->CheckArcade();
	}

	this->_settings->SetLocation(this->GetWidth() - 116, 8);
	this->_settings->SetSize(100, 45);
	
	this->_title->SetLocation(this->GetWidth() >> 1, 0);
	this->_title->SetSize(this->GetWidth() >> 1, 69);
	this->_browser->SetLocation(this->GetWidth() >> 1, 69);
	this->_browser->SetSize(this->GetWidth() >> 1, this->GetHeight() - (69 + 25));
	this->_actual->_page->SetWidth(this->_browser->GetWidth());

	for (int i = 0; i < this->_actual->_page->GetControls()->GetCount(); i++) {
		Control * child = (Control *)this->_actual->_page->GetControls()->Get(i);
		child->SetWidth(this->_browser->GetWidth() - 100);
	}

	this->_fade.SetBounds(0, 0, this->GetWidth(), this->GetHeight());

	if (this->_arcade) {
		if (this->_fade.IsFullScreen()) {
			this->_arcade->SetBounds(0, 0, this->GetWidth(), this->GetHeight());
		} else {
			this->_arcade->SetLocation(BORDERMARGIN, this->_browser->GetTop());
			this->_arcade->SetSize((this->GetWidth() >> 1) - BORDERMARGIN, this->_browser->GetHeight());
		}
	}
}

void StationUI::SelectChild(NodeFile * node) {
	if (node->_directory) {
		this->_actual = node;
		this->RefreshList();
		this->_actual->_page->GetFocused()->SetFocus(true);
		this->UpdateTitle();
	} else {
		if (node->_emulator != Types::Undefined) {
			if (!this->_fade.IsStopped())
				return;
			if (this->GetControls()->IndexOf(&this->_fade) == -1)
				this->GetControls()->Add(&this->_fade);

			this->GetControls()->MoveToEnd(&this->_fade);
			this->_arcade->SetTabStop(true);
			this->_arcade->SetFocus();
			this->_fade.ShowFade();
		}
	}
}

void StationUI::SelectParent() {
	if (this->_actual != NULL) {
		if (this->_actual->_parent != NULL) {
			this->_actual = this->_actual->_parent;
			this->_browser->SetPage(this->_actual->_page);
			this->_actual->_page->GetFocused()->SetFocus(true);
			this->UpdateTitle();
		}
	}
}

void StationUI::UpdateTitle() {
	if (this->_actual == this->_root)
		this->_title->SetText("StationTV");
	else
		this->_title->SetText(this->_actual->_button->GetText());
}

void StationUI::CheckArcade() {
	MenuButton * c = (MenuButton *) this->_actual->_page->GetFocused();
	if (c)
		c->CheckArcade();
}

void StationUI::SetArcade(Emulators::ArcadeContainer * arcade) {
	if (this->_arcade == arcade)
		return;

	this->GetControls()->Replace(this->_arcade, arcade);

	if (this->_arcade) {
		this->_arcade->SetSoundEnabled(false);
		this->_arcade = NULL;
	}

	if (arcade) {
		this->_arcade = arcade;
		this->_arcade->SetSoundEnabled(true);
	}
}

void StationUI::SetArcadeFullScreen() {
	this->GetControls()->Remove(&this->_fade);
	this->_title->SetVisible(false);
	this->_browser->SetVisible(false);
}

void StationUI::ExitingArcade() {
	if (!this->_fade.IsStopped())
		return;

	this->_title->SetVisible(true);
	this->_browser->SetVisible(true);
	this->_fade.HideFade();
	this->GetControls()->Add(&this->_fade);
	this->_arcade->SetTabStop(false);
	Control * c = this->_actual->_page->GetFocused();
	c->SetFocus(true);
}

void StationUI::ExitArcade() {
	this->GetControls()->Remove(&this->_fade);
}

bool StationUI::OnKeyPress(Keys::Enum key) {
	if (this->_arcade && (this->_arcade->GetType() == Types::Chip8))
		return this->_arcade->OnKeyPress(key);

	return Control::OnKeyPress(key);
}

bool StationUI::OnKeyUp(Keys::Enum key) {
	if (this->_arcade && (this->_arcade->GetType() == Types::Chip8))
		return this->_arcade->OnKeyUp(key);

	return Control::OnKeyUp(key);
}

/********************************* FadePanel **********************************/

FadePanel::FadePanel() {
	this->_station = NULL;
	this->_showing = false;
	this->_status = 0.0f;
}

FadePanel::~FadePanel() {
}

void FadePanel::ShowFade() {
	this->_showing = true;
}

void FadePanel::HideFade() {
	this->_showing = false;
}

void FadePanel::OnTick() {
	if (this->_showing) {
		this->_status += 10;
		if (Math::Round(this->_status) >= 200.0f) {
			this->_status = 200.0f;
			this->_station->SetArcadeFullScreen();
		}
	} else {
		this->_status -= 10;
		if (Math::Round(this->_status) <= 0.0f) {
			this->_status = 0.0f;
			this->_station->ExitArcade();
		}
	}

	if (this->_status <= 100.0f)
		this->SetBackColor(Color::FromArgb(this->_status * 2.55f, 0, 0, 0));
	else
		this->SetBackColor(Color::FromArgb((200.0f - this->_status) * 2.55f, 0, 0, 0));
}

/********************************** NodeFile **********************************/

NodeFile::NodeFile() {
	this->_parent = 0;
	this->_childList = 0;
	this->_directory = true;
	this->_emulator = Types::Undefined;
	this->_button = NULL;
	this->_page = NULL;
	this->_arcade = NULL;
}

NodeFile::~NodeFile() {
	// printf("%d) ~NodeFile:  %s\n", this->_emulator, this->_path.ToCharArray());
	if (this->_childList) {
		for (int i = 0; i < this->_childList->GetCount(); i++) {
			NodeFile * object = (NodeFile *)this->_childList->GetByIndex(i);
			delete object;
		}

		delete this->_childList;
	}

	if (this->_button)
		delete this->_button;
}
