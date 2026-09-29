/**
 * awui/Windows/Forms/Station/SettingsUI.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "SettingsUI.h"

#include <awui/Console.h>
#include <awui/Core/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/String.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Station/Browser.h>
#include <awui/Windows/Forms/Station/Page.h>
#include <awui/Windows/Forms/Station/Settings/ConfigButton.h>
#include <awui/Windows/Forms/Station/Settings/TypeConfigButton.h>

#include <fstream>
#include <iostream>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Station;
using namespace awui::Windows::Forms::Station::Settings;

#define MENUBUTTONHEIGHT 70

SettingsUI::SettingsUI() {
	m_class = Classes::SettingsUI;
}

bool SettingsUI::IsClass(Classes objectClass) const {
	return (objectClass == Classes::SettingsUI) || Control::IsClass(objectClass);
}

void SettingsUI::InitializeComponent() {
	// Si el fichero falta o está mal escrito se muestra el menú vacío en vez de abortar (parse sin excepciones)
	std::ifstream i("menu-settings.json");
	json j = i ? json::parse(i, nullptr, false) : json();
	if (!j.is_array()) {
		Console::Error->WriteLine("menu-settings.json no existe o no es válido (se espera un array)");
		j = json::array();
	}

	Font font = Font("Liberation Sans", 40, FontStyle::Bold);
	m_title = new Label();
	m_title->SetText("Settings");
	m_title->SetTextAlign(ContentAlignment::BottomCenter);
	m_title->SetFont(font);
	m_title->SetDock(DockStyle::None);
	m_title->SetForeColor(Color::FromArgb(120, 120, 120));
	AddWidget(m_title);

	Page *page = ProcessJson(j);

	m_browser = new Browser();
	m_browser->SetDock(DockStyle::None);
	AddWidget(m_browser);
	m_browser->SetPage(page);

	page->SetWidth(m_browser->GetWidth());
}

// types:
//    group
//    boolean
//    list
//    label

Page *SettingsUI::ProcessJson(const json &j, int depth) {
	// Siempre se devuelve una página, aunque quede vacía (quien llama la usa sin comprobar)
	Page *page = new Page();
	bool added = false;
	int posY = 25;
	if (j.is_array()) {
		// page->SetBackColor(Color::FromArgb(255, 0, 0));

		for (const auto &element : j) {
			if (!element.contains("type")) {
				continue;
			}

			TypeButton type = element["type"] == "group" ? TypeButton::Group : (element["type"] == "boolean" ? TypeButton::Boolean : (element["type"] == "list" ? TypeButton::List : TypeButton::Label));

			switch (type) {
				case TypeButton::Group: {
					// Un grupo sin nombre de texto no se muestra (y no se crea el botón, que quedaría sin dueño)
					if (element.contains("name") && element["name"].is_string()) {
						ConfigButton *button = new ConfigButton(TypeButton::Group);
						// std::cout << std::string(depth * 2, ' ') << element["name"] << ":" << std::endl;
						std::string test = element["name"].get<std::string>();
						button->SetText(test.c_str());
						button->SetHeight(MENUBUTTONHEIGHT);
						button->SetGroup(true);
						button->SetLocation(40, posY);
						posY += MENUBUTTONHEIGHT;

						page->AddWidget(button);

						if (!added) {
							button->SetFocus();
							added = true;
						}

						if (element.contains("items")) {
							button->SetSubPage(ProcessJson(element["items"], depth + 1));
							button->AddOnClickListener(this);
						}
					}
				} break;
				case TypeButton::Boolean:
					// std::cout << std::string(depth * 2, ' ');

					if (element.contains("name")) {
						// std::cout << element["name"] << ": " ;
					}
					if (element.contains("key")) {
						// std::cout << element["key"] << ": " ;
					}
					if (element.contains("defaultValue")) {
						// std::cout << element["defaultValue"] << std::endl;
					}
					break;
				case TypeButton::List:
					// std::cout << std::string(depth * 2, ' ');
					if (element.contains("name")) {
						// std::cout << element["name"] << ": " ;
					}
					if (element.contains("key")) {
						// std::cout << element["key"] << ": " ;
					}
					if (element.contains("defaultValue")) {
						// std::cout << element["defaultValue"] << std::endl;
					}

					// Con el json constante, pedir una clave que no existe es comportamiento indefinido
					if (!element.contains("options"))
						break;

					for (const auto &option : element["options"]) {
						// std::cout << std::string((depth + 1) * 2, ' ');
						if (option.contains("code")) {
							// std::cout << option["code"] << ": " ;
						}
						if (option.contains("name")) {
							// std::cout << option["name"];
						}
						// std::cout << std::endl;
					}
					break;
				case TypeButton::Label:
					// std::cout << std::string(depth * 2, ' ');

					if (element.contains("name")) {
						// std::cout << element["name"] << ": " ;
					}
					if (element.contains("key")) {
						// std::cout << element["key"] << ": " ;
					}
					if (element.contains("defaultValue")) {
						// std::cout << element["defaultValue"] << std::endl;
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
	Page *page = m_browser->GetPage();
	if (page) {
		// Console::WriteLine("%d", m_browser->GetWidth());
		page->SetWidth(m_browser->GetWidth());

		for (int i = 0; i < page->GetCount(); i++) {
			Control *child = page->Get(i);
			child->SetWidth(m_browser->GetWidth() - 80);
		}
	}
}

void SettingsUI::OnOk(Control *sender) {
	if (sender->IsClass(Classes::ConfigButton)) {
		ConfigButton *button = (ConfigButton *) sender;
		switch (button->GetTypeButton()) {
			case TypeButton::Group:
				Page *page = button->GetSubPage();
				m_browser->SetPage(page);
				page->SetWidth(m_browser->GetWidth());
				CheckMouseControl();
				break;
		}
	}
}

void SettingsUI::OnMenu(Control *sender) {
}