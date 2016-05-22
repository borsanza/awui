/**
 * awui/Windows/Station/SearchFiles.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

 #include "SearchFiles.h"

#include <dirent.h>
#include <string.h>

using namespace awui::Windows::Station;

SearchFiles::SearchFiles() {
	this->_root = 0;
}

SearchFiles::~SearchFiles() {
	// printf("~SearchFiles\n");
	if (this->_root)
		delete this->_root;
}

void SearchFiles::SetPath(const String path) {
	this->_path = path;
}

void SearchFiles::Clear() {
	if (this->_root) {
		delete this->_root;
		this->_root = 0;
	}
}

void SearchFiles::RecursiveSearch(NodeFile * parent) {
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

bool SearchFiles::Minimize(NodeFile * parent) {
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

void SearchFiles::Refresh() {
	this->Clear();
	this->_root = new NodeFile();
	this->_root->_path = this->_path;
	this->_root->_emulator = 0;
	this->RecursiveSearch(this->_root);

	// printf("\nMinimize:\n\n");
	while (this->Minimize(this->_root));
	// printf("\nFinish:\n\n");
}

/********************************** NodeFile **********************************/

NodeFile::NodeFile() {
	this->_childList = 0;
	this->selectedChild = 0;
	this->_directory = true;
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
}
