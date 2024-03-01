/**
 * awui/Windows/Forms/Station/SettingsUI.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "SettingsUI.h"

#include <awui/Console.h>
#include <awui/String.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Station/Browser/Browser.h>
#include <awui/Windows/Forms/Station/Browser/Page.h>
#include <awui/Windows/Forms/Station/Settings/ConfigButton.h>

#include <fstream>
#include <iostream>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Station;
using namespace awui::Windows::Forms::Station::Browser;
using namespace awui::Windows::Forms::Station::Settings;

#define MENUBUTTONHEIGHT 70

SettingsUI::SettingsUI() {
}

void SettingsUI::InitializeComponent() {
	std::ifstream i("menu-settings.json");
	json j;
	i >> j;

	Font font = Font("Liberation Sans", 40, FontStyle::Bold);
	m_title = new Label();
	m_title->SetText("Settings");
	m_title->SetTextAlign(ContentAlignment::BottomCenter);
	m_title->SetFont(font);
	m_title->SetDock(DockStyle::None);
	m_title->SetForeColor(Color::FromArgb(120, 120, 120));
	GetControls()->Add(m_title);

	// SetBackColor(Color::FromArgb(0, 255, 0));
	SetTabStop(false);
	Browser::Page * page = ProcessJson(j);

	m_browser = new Browser::Browser();
	m_browser->SetDock(DockStyle::None);
	GetControls()->Add(m_browser);
	m_browser->SetPage(page);
	// m_browser->SetBackColor(Color::FromArgb(0, 0, 255));

	page->SetWidth(m_browser->GetWidth());
}

// types:
//    group
//    boolean
//    list
//    label

Browser::Page * SettingsUI::ProcessJson(const json &j, int depth) {
	Browser::Page * page = nullptr;
	bool added = false;
	int posY = 25;
	if (j.is_array()) {
		page = new Browser::Page();
		// page->SetBackColor(Color::FromArgb(255, 0, 0));

		for (const auto& element : j) {
			if (!element.contains("type")) {
				continue;
			}

			int type =   element["type"] == "group" ?   1 :
						(element["type"] == "boolean" ? 2 :
						(element["type"] == "list" ?    3 : 4));

			switch (type) {
				case 1: // Group
				{
					ConfigButton * button = new ConfigButton();
					if (element.contains("name")) {
						std::cout << std::string(depth * 2, ' ') << element["name"] << ":" << std::endl;
						std::string test = element["name"].get<std::string>();
						button->SetText(test.c_str());
						button->SetHeight(MENUBUTTONHEIGHT);
						button->SetGroup(true);
						button->SetLocation(40, posY);
						posY += MENUBUTTONHEIGHT;
						page->GetControls()->Add(button);
						if (!added) {
							button->SetFocus();
							added = true;
						}
					}
					if (element.contains("items")) {
						ProcessJson(element["items"], depth + 1);
					}
				}
					break;
				case 2: // Boolean
					std::cout << std::string(depth * 2, ' ');

					if (element.contains("name")) {
						std::cout << element["name"] << ": " ;
					}
					if (element.contains("key")) {
						std::cout << element["key"] << ": " ;
					}
					if (element.contains("defaultValue")) {
						std::cout << element["defaultValue"] << std::endl;
					}
					break;
				case 3: // List
					std::cout << std::string(depth * 2, ' ');
					if (element.contains("name")) {
						std::cout << element["name"] << ": " ;
					}
					if (element.contains("key")) {
						std::cout << element["key"] << ": " ;
					}
					if (element.contains("defaultValue")) {
						std::cout << element["defaultValue"] << std::endl;
					}

					for (const auto& option : element["options"]) {
						std::cout << std::string((depth + 1) * 2, ' ');
						if (option.contains("code")) {
							std::cout << option["code"] << ": " ;
						}
						if (option.contains("name")) {
							std::cout << option["name"];
						}
						std::cout << std::endl;
					}
					break;
				case 4: // Label
					std::cout << std::string(depth * 2, ' ');

					if (element.contains("name")) {
						std::cout << element["name"] << ": " ;
					}
					if (element.contains("key")) {
						std::cout << element["key"] << ": " ;
					}
					if (element.contains("defaultValue")) {
						std::cout << element["defaultValue"] << std::endl;
					}
					break;
			}
		}
	}

	page->SetHeight(posY + 25);

	return page;
}

void SettingsUI::OnTick(float deltaSeconds) {
	m_title->SetLocation(0, 21);
	m_title->SetSize(GetWidth(), 69);
	m_browser->SetLocation((this->GetWidth() / 2.0) + 42, 118);
	m_browser->SetSize((this->GetWidth() / 2.0) - 66, this->GetHeight() - 260);
	Page * page = m_browser->GetPage();
	if (page) {
		// Console::WriteLine("%d", m_browser->GetWidth());
		page->SetWidth(m_browser->GetWidth());

		for (int i = 0; i < page->GetControls()->GetCount(); i++) {
			Control * child = (Control *)page->GetControls()->Get(i);
			child->SetWidth(m_browser->GetWidth() - 80);
		}
	}
}