/**
 * awui/UI/Station/Settings/SettingsStore.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "SettingsStore.h"

#include <awui/Configuration.h>
#include <awui/Console.h>
#include <awui/Localization.h>

#include <fstream>

using namespace awui;
using namespace awui::UI::Station::Settings;
using json = nlohmann::json;

#define MENU_FILE "menu-settings.json"

SettingsStore::SettingsStore() {
	m_defaults = json::object();
	m_options = json::object();

	// Si el fichero falta o está mal escrito el menú queda vacío y todo usa sus valores por defecto
	std::ifstream file(MENU_FILE);
	m_menu = file ? json::parse(file, nullptr, false) : json();
	if (!m_menu.is_array()) {
		Console::Error->WriteLine(MENU_FILE " no existe o no es válido (se espera un array)");
		m_menu = json::array();
	}

	CollectDefaults(m_menu);
}

SettingsStore &SettingsStore::Instance() {
	static SettingsStore instance;
	return instance;
}

void SettingsStore::CollectDefaults(const json &items) {
	if (!items.is_array())
		return;

	for (const auto &item : items) {
		if (!item.is_object())
			continue;

		if (item.contains("key") && item["key"].is_string()) {
			std::string key = item["key"].get<std::string>();
			if (item.contains("defaultValue"))
				m_defaults[key] = item["defaultValue"];

			if (item.contains("options")) {
				m_options[key] = json::array();
				for (const auto &option : GetOptions(item))
					m_options[key].push_back(option.first);
			}
		}

		if (item.contains("items"))
			CollectDefaults(item["items"]);
	}
}

bool SettingsStore::GetBool(const std::string &key) {
	json value = Configuration::getInstance(s_valuesFile).Get(key);
	if (value.is_boolean())
		return value.get<bool>();

	return m_defaults.contains(key) && m_defaults[key].is_boolean() ? m_defaults[key].get<bool>() : false;
}

std::string SettingsStore::GetString(const std::string &key) {
	json value = Configuration::getInstance(s_valuesFile).Get(key);
	if (value.is_string()) {
		bool valid = !m_options.contains(key);
		if (!valid) {
			for (const auto &code : m_options[key])
				valid |= (code == value);
		}

		if (valid)
			return value.get<std::string>();
	}

	return m_defaults.contains(key) && m_defaults[key].is_string() ? m_defaults[key].get<std::string>() : "";
}

void SettingsStore::SetBool(const std::string &key, bool value) {
	Configuration::getInstance(s_valuesFile).Write(key, value);
}

void SettingsStore::SetString(const std::string &key, const std::string &value) {
	Configuration::getInstance(s_valuesFile).Write(key, value);
}

std::vector<std::pair<std::string, std::string>> SettingsStore::GetOptions(const json &item) {
	std::vector<std::pair<std::string, std::string>> options;
	if (!item.contains("options"))
		return options;

	const json &list = item["options"];
	if (list.is_string() && (list.get<std::string>() == "languages")) {
		// El nombre de cada idioma se muestra en su propio idioma (no es una clave de traducción)
		for (const auto &language : Localization::GetLanguages())
			options.push_back({language.first, language.second.ToCharArray()});

		return options;
	}

	if (!list.is_array())
		return options;

	for (const auto &option : list) {
		if (option.is_object() && option.contains("code") && option["code"].is_string() && option.contains("name") && option["name"].is_string())
			options.push_back({option["code"].get<std::string>(), option["name"].get<std::string>()});
	}

	return options;
}
