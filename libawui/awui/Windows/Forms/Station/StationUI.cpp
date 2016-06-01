/**
 * awui/Windows/Forms/Station/StationUI.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

 #include "StationUI.h"

#include <awui/Drawing/Font.h>
#include <awui/Effects/Effect.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <dirent.h>
#include <string.h>

/**
 * Chip8: *.ch8, *.c8x
 * Master System: *.sms, *.sg
 * Spectrum: *.rom, *.tap
 */

using namespace awui::Drawing;
using namespace awui::Effects;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Station;

/********************************** NodeFile **********************************/

NodeFile::NodeFile() {
	this->_parent = 0;
	this->_childList = 0;
	this->_selectedChild = 0;
	this->_directory = true;
	this->_emulator = 0;
	this->_label = 0;
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

	if (this->_label) {
		this->_label->GetParent()->GetControls()->Remove(this->_label);
		delete this->_label;
	}
}

/********************************* StationUI **********************************/

StationUI::StationUI() {
	this->_root = NULL;

	this->_margin = 2;
	this->_effect = new EffectLinear();
	this->SetTabStop(false);
	this->_lastControl = NULL;
	this->_lastTime = 0;
	this->_initPos = 0;
	this->_selected = -1;
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

			child->_label = new MenuButton();
			child->_label->SetFont(new Font("Monospace", 28, FontStyle::Bold));
			child->_label->SetDock(DockStyle::None);
			String name = child->_name;
			name = name.Substring(0, name.LastIndexOf("."));
			child->_label->SetText(name);
			child->_label->SetHeight(61);
			child->_label->SetWidth(510);
			this->GetControls()->Add(child->_label);

			if (parent->_emulator == 0) {
				if (child->_name == "chip8")
					child->_emulator = 1;
				if (child->_name == "gamegear")
					child->_emulator = 2;
				if (child->_name == "mastersystem")
					child->_emulator = 3;
				if (child->_name == "zxspectrum")
					child->_emulator = 4;
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

	// printf("\nMinimize:\n\n");
	while (this->Minimize(this->_root));
	// printf("\nFinish:\n\n");
}

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

void StationUI::OnTick() {
	for (int i = 0; i< this->GetControls()->GetCount(); i++) {
		MenuButton * w = (MenuButton *)this->GetControls()->Get(i);
		w->SetWidth(633); // w->GetLabelWidth() + 18 * 2);
	}

	int posSelected = this->GetControls()->IndexOf(Form::GetControlSelected());

	if (posSelected != -1)
		if (this->_selected != posSelected)
			this->_selected = posSelected;

	if (this->_selected == -1)
		this->_selected = 0;

	if (this->_selected >= this->GetControls()->GetCount())
		return;

	Control * w = (Control *)this->GetControls()->Get(this->_selected);

	int top = (this->GetHeight() - w->GetHeight()) >> 1;
	w->SetLocationGo(70, top);
	w->SetForeColor(Color::FromArgb(255, 255, 255));

	int y = w->GetTop() + w->GetHeight() + this->_margin;
	for (int i = this->_selected + 1; i< this->GetControls()->GetCount(); i++) {
		MenuButton * w2 = (MenuButton *)this->GetControls()->Get(i);
		y += this->_margin;
		w2->SetLocation(70, y);
		w2->SetForeColor(Color::FromArgb(199, 199, 199));
		y += w2->GetHeight() + this->_margin;
	}

	y = w->GetTop() - this->_margin;
	for (int i = this->_selected - 1; i >= 0; i--) {
		Control * w2 = (Control *)this->GetControls()->Get(i);
		y -= (this->_margin + w2->GetHeight());
		w2->SetLocation(70, y);
		w2->SetForeColor(Color::FromArgb(199, 199, 199));
		y -= this->_margin;
	}
}
