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
#include <awui/Windows/Forms/Label.h>
#include <awui/Windows/Forms/Station/Browser.h>
#include <awui/Windows/Forms/Station/Page.h>
#include <awui/Windows/Forms/Station/Settings/ConfigButton.h>
#include <awui/Windows/Forms/Station/Settings/SettingsStore.h>
#include <awui/Windows/Forms/Station/Settings/TypeConfigButton.h>
#include <awui/Windows/Forms/TextRenderer.h>

#include <algorithm>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Station;
using namespace awui::Windows::Forms::Station::Settings;

#define MENUBUTTONHEIGHT 70
#define DESCRIPTIONHEIGHT 36

SettingsUI::SettingsUI() {
	m_class = Classes::SettingsUI;
	m_browser = nullptr;
	m_title = nullptr;
	m_rootPage = nullptr;
	m_rebuild = false;
	m_exitListener = nullptr;

	for (int i = 0; i < DescriptionLines; i++)
		m_description[i] = nullptr;
}

SettingsUI::~SettingsUI() {
	DeletePages();
}

bool SettingsUI::IsClass(Classes objectClass) const {
	return (objectClass == Classes::SettingsUI) || Control::IsClass(objectClass);
}

void SettingsUI::InitializeComponent() {
	Font font = Font("Liberation Sans", 40, FontStyle::Bold);
	m_title = new Label();
	m_title->SetTextAlign(ContentAlignment::BottomCenter);
	m_title->SetFont(font);
	m_title->SetDock(DockStyle::None);
	m_title->SetForeColor(Color::FromArgb(120, 120, 120));
	AddWidget(m_title);

	Font fontDescription = Font("Liberation Sans", 24, FontStyle::Regular);
	for (int i = 0; i < DescriptionLines; i++) {
		m_description[i] = new Label();
		m_description[i]->SetFont(fontDescription);
		m_description[i]->SetDock(DockStyle::None);
		m_description[i]->SetTextAlign(ContentAlignment::MiddleLeft);
		m_description[i]->SetForeColor(Color::FromArgb(151, 151, 151));
		AddWidget(m_description[i]);
	}

	m_browser = new Browser();
	m_browser->SetDock(DockStyle::None);
	AddWidget(m_browser);

	Build();
}

void SettingsUI::Build() {
	m_rootPage = ProcessJson(SettingsStore::Instance().GetMenu());
	ShowPage(m_rootPage);
}

// Las páginas no se borran solas: solo la que se ve cuelga del Browser
void SettingsUI::DeletePages() {
	if (m_browser)
		m_browser->SetPage(nullptr);

	for (Page *page : m_pages)
		delete page;

	m_pages.clear();
	m_path.clear();
	m_rootPage = nullptr;
}

void SettingsUI::ShowPage(Page *page) {
	m_browser->SetPage(page);
	page->SetWidth(m_browser->GetWidth());
	CheckMouseControl();
	UpdateTitle();
}

void SettingsUI::UpdateTitle() {
	if (m_path.empty())
		m_title->SetText(SettingsStore::Instance().Text("settings"));
	else
		m_title->SetText(m_path.back()->GetText());
}

// types:
//    group:   name, items
//    boolean: key, name, description, defaultValue
//    list:    key, name, description, defaultValue, options [{code, name}]
//    label:   name, description, defaultValue (solo se muestra)
// Los textos (name, description) pueden ser una cadena o un objeto por idioma
Page *SettingsUI::ProcessJson(const json &j) {
	SettingsStore &store = SettingsStore::Instance();

	// Siempre se devuelve una página, aunque quede vacía (quien llama la usa sin comprobar)
	Page *page = new Page();
	m_pages.push_back(page);

	bool added = false;
	int posY = 25;
	if (j.is_array()) {
		for (const auto &element : j) {
			if (!element.is_object() || !element.contains("type") || !element["type"].is_string() || !element.contains("name")) {
				continue;
			}

			std::string typeName = element["type"].get<std::string>();
			std::string key = (element.contains("key") && element["key"].is_string()) ? element["key"].get<std::string>() : "";
			ConfigButton *button = nullptr;

			if (typeName == "group") {
				if (!element.contains("items") || !element["items"].is_array())
					continue;

				button = new ConfigButton(TypeButton::Group);
				button->SetSubPage(ProcessJson(element["items"]));
			} else if (typeName == "boolean") {
				if (key.empty())
					continue;

				button = new ConfigButton(TypeButton::Boolean);
				button->SetKey(key);
				button->SetBoolValue(store.GetBool(key), store.Text("on"), store.Text("off"));
			} else if (typeName == "list") {
				if (key.empty() || !element.contains("options") || !element["options"].is_array())
					continue;

				std::vector<std::pair<std::string, String>> options;
				for (const auto &option : element["options"]) {
					if (option.is_object() && option.contains("code") && option["code"].is_string() && option.contains("name"))
						options.push_back({option["code"].get<std::string>(), store.Translate(option["name"])});
				}

				if (options.empty())
					continue;

				button = new ConfigButton(TypeButton::List);
				button->SetKey(key);
				button->SetOptions(options, store.GetString(key));
			} else if (typeName == "label") {
				button = new ConfigButton(TypeButton::Label);
				if (element.contains("defaultValue"))
					button->SetValueText(store.Translate(element["defaultValue"]));
			} else {
				continue;
			}

			button->SetText(store.Translate(element["name"]));
			if (element.contains("description"))
				button->SetDescription(store.Translate(element["description"]));

			button->SetHeight(MENUBUTTONHEIGHT);
			button->SetLocation(40, posY);
			button->AddOnClickListener(this);
			posY += MENUBUTTONHEIGHT;

			page->AddWidget(button);

			if (!added) {
				button->SetFocus();
				added = true;
			}
		}
	}

	page->SetHeight(posY + 25);

	return page;
}

// Parte la descripción del elemento con el foco en líneas que quepan a la izquierda del menú
void SettingsUI::UpdateDescription() {
	String text;
	Form *form = GetForm();
	Control *focused = form ? form->GetChildFocused() : nullptr;
	if (focused && focused->IsClass(Classes::ConfigButton) && (focused->GetParent() == m_browser->GetPage()))
		text = ((ConfigButton *) focused)->GetDescription();

	int left = 66;
	int width = std::max(0, m_browser->GetLeft() - 42 - left);
	int top = m_browser->GetTop() + 25;

	for (int i = 0; i < DescriptionLines; i++) {
		m_description[i]->SetLocation(left, top + (i * DESCRIPTIONHEIGHT));
		m_description[i]->SetSize(width, DESCRIPTIONHEIGHT);
	}

	if (text == m_lastDescription)
		return;

	m_lastDescription = text;

	Font *font = m_description[0]->GetFont();
	std::string words = text.ToCharArray();
	std::string line;
	int lineIndex = 0;
	size_t pos = 0;
	while ((pos < words.size()) && (lineIndex < DescriptionLines)) {
		size_t end = words.find(' ', pos);
		if (end == std::string::npos)
			end = words.size();

		std::string word = words.substr(pos, end - pos);
		std::string candidate = line.empty() ? word : line + " " + word;
		if (!line.empty() && (TextRenderer::GetMeasureText(candidate.c_str(), font).GetWidth() > width)) {
			m_description[lineIndex++]->SetText(line.c_str());
			line = word;
		} else {
			line = candidate;
		}

		pos = end + 1;
	}

	if (lineIndex < DescriptionLines)
		m_description[lineIndex++]->SetText(line.c_str());

	while (lineIndex < DescriptionLines)
		m_description[lineIndex++]->SetText("");
}

// Vuelve a crear el menú (al cambiar de idioma) conservando los grupos abiertos y la fila con el foco
void SettingsUI::Rebuild() {
	std::vector<int> path;
	Page *page = m_rootPage;
	for (ConfigButton *group : m_path) {
		path.push_back(page->IndexOf(group));
		page = group->GetSubPage();
	}

	int focusedIndex = -1;
	Page *current = m_browser->GetPage();
	if (current && current->GetFocused())
		focusedIndex = current->IndexOf(current->GetFocused());

	DeletePages();
	m_rootPage = ProcessJson(SettingsStore::Instance().GetMenu());

	page = m_rootPage;
	for (int index : path) {
		if ((index < 0) || (index >= page->GetCount()))
			break;

		ConfigButton *group = (ConfigButton *) page->Get(index);
		if (!group->IsGroup())
			break;

		m_path.push_back(group);
		page = group->GetSubPage();
	}

	if ((focusedIndex >= 0) && (focusedIndex < page->GetCount()))
		page->Get(focusedIndex)->SetFocus();

	ShowPage(page);
	m_lastDescription = "";
}

void SettingsUI::OnTick(float deltaSeconds) {
	if (m_rebuild) {
		m_rebuild = false;
		Rebuild();
	}

	m_title->SetLocation(0, 21);
	m_title->SetSize(GetWidth(), 69);
	m_browser->SetLocation((this->GetWidth() / 2.0) + 42, 118);
	m_browser->SetSize((this->GetWidth() / 2.0) - 66, this->GetHeight() - 260);
	Page *page = m_browser->GetPage();
	if (page) {
		page->SetWidth(m_browser->GetWidth());

		for (int i = 0; i < page->GetCount(); i++) {
			Control *child = page->Get(i);
			child->SetWidth(m_browser->GetWidth() - 80);
		}
	}

	UpdateDescription();
}

void SettingsUI::OnOk(Control *sender) {
	if (!sender->IsClass(Classes::ConfigButton))
		return;

	SettingsStore &store = SettingsStore::Instance();
	ConfigButton *button = (ConfigButton *) sender;
	switch (button->GetTypeButton()) {
		case TypeButton::Group:
			m_path.push_back(button);
			ShowPage(button->GetSubPage());
			return;
		case TypeButton::Boolean:
			store.SetBool(button->GetKey(), button->GetBoolValue());
			break;
		case TypeButton::List:
			store.SetString(button->GetKey(), button->GetListValue());
			// Los textos del menú dependen del idioma: se rehace en el siguiente tick (ahora se está dentro de un botón)
			if (button->GetKey() == "language")
				m_rebuild = true;
			break;
		default:
			return;
	}

	if (m_onChanged)
		m_onChanged();
}

void SettingsUI::OnMenu(Control *sender) {
	if (m_path.empty()) {
		if (m_exitListener)
			m_exitListener->OnExit(this);

		return;
	}

	// Al volver, el foco queda en el grupo del que se sale
	ConfigButton *group = m_path.back();
	m_path.pop_back();
	group->SetFocus();
	ShowPage(m_path.empty() ? m_rootPage : m_path.back()->GetSubPage());
}
