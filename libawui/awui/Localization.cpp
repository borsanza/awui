/**
 * awui/Localization.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "Localization.h"

#include <awui/Console.h>

#include <algorithm>
#include <dirent.h>
#include <fstream>

using namespace awui;
using json = nlohmann::json;

const char *Localization::FallbackLanguage = "en_US";

void Localization::SetDirectory(const std::string &directory) {
	m_directory = directory;
	m_fallback = LoadFile(FallbackLanguage);
	m_texts = LoadFile(m_language);
}

json Localization::LoadFile(const std::string &code) {
	if (code.empty())
		return json::object();

	std::string path = m_directory + "/" + code + ".json";
	std::ifstream file(path);
	json texts = file ? json::parse(file, nullptr, false) : json();
	if (!texts.is_object()) {
		Console::Error->WriteLine(String("No se pueden cargar las traducciones de ") + path.c_str());
		return json::object();
	}

	return texts;
}

void Localization::SetLanguage(const std::string &code) {
	if ((code == m_language) && !m_texts.empty())
		return;

	m_language = code;
	if (m_fallback.empty())
		m_fallback = LoadFile(FallbackLanguage);

	m_texts = (code == FallbackLanguage) ? m_fallback : LoadFile(code);
}

String Localization::Tr(const std::string &key) {
	for (const json *texts : {&m_texts, &m_fallback}) {
		auto it = texts->find(key);
		if ((it != texts->end()) && it->is_string())
			return it->get<std::string>().c_str();
	}

	return key.c_str();
}

std::vector<std::pair<std::string, String>> Localization::GetLanguages() {
	std::vector<std::pair<std::string, String>> languages;

	DIR *dir = opendir(m_directory.c_str());
	if (!dir)
		return languages;

	struct dirent *entry;
	while ((entry = readdir(dir)) != nullptr) {
		std::string name = entry->d_name;
		if ((name.size() <= 5) || (name.compare(name.size() - 5, 5, ".json") != 0))
			continue;

		std::string code = name.substr(0, name.size() - 5);
		json texts = LoadFile(code);
		String displayName = (texts.contains("language.name") && texts["language.name"].is_string()) ? String(texts["language.name"].get<std::string>().c_str()) : String(code.c_str());
		languages.push_back({code, displayName});
	}

	closedir(dir);

	std::sort(languages.begin(), languages.end(), [](const auto &a, const auto &b) { return a.first < b.first; });

	return languages;
}
