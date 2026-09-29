/**
 * awui/Windows/Forms/Station/Settings/SettingsStore.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "SettingsStore.h"

#include <awui/Configuration.h>
#include <awui/Console.h>

#include <fstream>

using namespace awui;
using namespace awui::Windows::Forms::Station::Settings;
using json = nlohmann::json;

#define MENU_FILE "menu-settings.json"
#define VALUES_FILE "settings.json"

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

			if (item.contains("options") && item["options"].is_array()) {
				m_options[key] = json::array();
				for (const auto &option : item["options"]) {
					if (option.is_object() && option.contains("code") && option["code"].is_string())
						m_options[key].push_back(option["code"]);
				}
			}
		}

		if (item.contains("items"))
			CollectDefaults(item["items"]);
	}
}

bool SettingsStore::GetBool(const std::string &key) {
	json value = Configuration::getInstance(VALUES_FILE).Get(key);
	if (value.is_boolean())
		return value.get<bool>();

	return m_defaults.contains(key) && m_defaults[key].is_boolean() ? m_defaults[key].get<bool>() : false;
}

std::string SettingsStore::GetString(const std::string &key) {
	json value = Configuration::getInstance(VALUES_FILE).Get(key);
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
	Configuration::getInstance(VALUES_FILE).Write(key, value);
}

void SettingsStore::SetString(const std::string &key, const std::string &value) {
	Configuration::getInstance(VALUES_FILE).Write(key, value);
}

String SettingsStore::Translate(const json &text) {
	if (text.is_string())
		return text.get<std::string>().c_str();

	if (!text.is_object() || text.empty())
		return "";

	// Idioma elegido; si falta, inglés; si tampoco, el primero que haya
	std::string language = GetString("language");
	for (const std::string &code : {language, std::string("en_US")}) {
		if (text.contains(code) && text[code].is_string())
			return text[code].get<std::string>().c_str();
	}

	const json &first = text.begin().value();
	return first.is_string() ? first.get<std::string>().c_str() : "";
}

String SettingsStore::Text(const std::string &id) {
	static const json texts = {
		{"settings", {{"en_US", "Settings"}, {"es_ES", "Ajustes"}}},
		{"on", {{"en_US", "On"}, {"es_ES", "Sí"}}},
		{"off", {{"en_US", "Off"}, {"es_ES", "No"}}},
	};

	return texts.contains(id) ? Translate(texts[id]) : String(id.c_str());
}
