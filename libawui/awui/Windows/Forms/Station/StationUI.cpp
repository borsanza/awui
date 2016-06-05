/**
 * awui/Windows/Forms/Station/StationUI.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

 #include "StationUI.h"

#include <awui/Math.h>
#include <awui/Drawing/Font.h>
#include <awui/Effects/Effect.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Gradient.h>
#include <awui/Windows/Forms/Station/Browser/Browser.h>
#include <awui/Windows/Forms/Station/Browser/Page.h>

#include <dirent.h>

#define BORDERMARGIN 50
#define MENUBUTTONHEIGHT 70

using namespace awui::Drawing;
using namespace awui::Effects;
using namespace awui::Windows::Forms::Station;
using namespace awui::Windows::Forms::Station::Browser;

StationUI::StationUI() {
	this->_fade.SetStationUI(this);
	this->_arcade = NULL;
	this->_root = NULL;

	this->SetTabStop(false);

	this->_title = new Label();
	this->_title->SetText("StationTV");
	this->_title->SetTextAlign(ContentAlignment::BottomCenter);
	this->_title->SetFont(new Font("Liberation Sans", 38, FontStyle::Bold));
	this->_title->SetDock(DockStyle::None);

	this->_browser = new Browser::Browser();
	this->GetControls()->Add(this->_title);
	this->GetControls()->Add(this->_browser);
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
			child->_button->SetFont(new Font("Liberation Sans", 28, FontStyle::Bold));
			child->_button->SetDock(DockStyle::None);
			String name = child->_name;
			name = name.Substring(0, name.LastIndexOf("."));
			child->_button->SetText(name);

			if (parent->_emulator == 0) {
				if (child->_name == "chip8") {
					child->_emulator = 1;
					child->_button->SetText("CHIP-8");
				}

				if (child->_name == "gamegear") {
					child->_emulator = 2;
					child->_button->SetText("Game Gear");
				}

				if (child->_name == "mastersystem") {
					child->_emulator = 3;
					child->_button->SetText("Master System");
				}

				if (child->_name == "zxspectrum") {
					child->_emulator = 4;
					child->_button->SetText("ZX Spectrum");
				}
			} else {
				child->_emulator = parent->_emulator;
			}

			// 4 Directory
			// 8 Files
			// if (dir->d_type == DT_REG)
				// printf("%s\n", newFile.ToCharArray());

			if (!parent->_childList)
				parent->_childList = new SortedList();

			parent->_childList->Add(&child->_name, child);

			child->_path = newFile;
			child->_parent = parent;
			child->_directory = dir->d_type == DT_DIR;

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
					case 1:
						if (child->_path.EndsWith("ch8")) continue;
						if (child->_path.EndsWith("c8x")) continue;
						break;
					case 2:
					case 3:
						if (child->_path.EndsWith("sms")) continue;
						if (child->_path.EndsWith("sg")) continue;
						break;
					case 4:
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
	this->_root->_emulator = 0;
	this->RecursiveSearch(this->_root);
	while (this->Minimize(this->_root));

	this->RefreshList();
}

/*
void StationUI::GetList(ArrayList * list, NodeFile * parent) {
	if (parent == 0)
		parent = this->_root;

	if (parent->_childList) {
		for (int i = 0; i < parent->_childList->GetCount(); i++) {
			NodeFile * child = (NodeFile *)parent->_childList->GetByIndex(i);
			if (child->_directory)
				GetList(list, child);
			else
				list->Add(&child->_path);
		}
	}
}
*/

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
}

void StationUI::ExitingArcade() {
	if (!this->_fade.IsStopped())
		return;

	this->_fade.HideFade();
	this->GetControls()->Add(&this->_fade);
	this->_arcade->SetTabStop(false);
	Control * c = this->_actual->_page->GetFocused();
	c->SetFocus(true);
}

void StationUI::ExitArcade() {
	this->GetControls()->Remove(&this->_fade);
}

/********************************* FadePanel **********************************/

FadePanel::FadePanel() {
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
	this->_emulator = 0;
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
