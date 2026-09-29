/**
 * awui/Configuration.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */
#include "Configuration.h"

#include <awui/Console.h>

#include <fstream>

using namespace awui;

awui::Configuration &Configuration::getInstance(const String &filePath) {
	std::string filePathStr = filePath.ToCharArray();
	static std::map<std::string, std::shared_ptr<Configuration>> instances;
	if (instances.find(filePathStr) == instances.end()) {
		// Aunque no se pueda cargar se guarda la instancia (vacía): quien llama la usa sin comprobar
		auto instance = std::shared_ptr<Configuration>(new Configuration(filePath));
		if (!instance->Load(filePath)) {
			Console::Error->WriteLine(String("Unable to load configuration from ") + filePath);
		}

		instances[filePathStr] = instance;
	}

	return *instances[filePathStr];
}

Configuration::Configuration(const String &filePath) {
	m_currentFilePath = filePath;
}

bool Configuration::Load(const String &filePath) {
	m_configJson = nlohmann::json::object();

	std::ifstream file(filePath.ToCharArray());
	if (file.is_open()) {
		// Sin excepciones: un fichero mal escrito se trata como vacío (se reescribe al guardar)
		nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
		if (!json.is_object()) {
			Console::Error->WriteLine(String("Invalid configuration in ") + filePath);
			return false;
		}

		m_configJson = json;
		return true;
	} else {
		std::ofstream outFile(filePath.ToCharArray());
		if (outFile) {
			nlohmann::json defaultConfig = nlohmann::json::object();
			outFile << defaultConfig.dump(4);
			outFile.close();
			m_configJson = defaultConfig;
			return true;
		}
	}

	return false;
}

void Configuration::Save() {
	if (m_currentFilePath.GetLength() > 0) {
		std::ofstream file(m_currentFilePath.ToCharArray());
		file << m_configJson.dump(4);
	}
}

// Si la clave falta o tiene otro tipo se usa (y se guarda) el valor por defecto

std::string Configuration::Read(const std::string &key, const char *defaultValue) {
	return Read(key, std::string(defaultValue));
}

std::string Configuration::Read(const std::string &key, const std::string &defaultValue) {
	if (m_configJson.contains(key) && m_configJson[key].is_string()) {
		return m_configJson[key].get<std::string>();
	}

	Write(key, defaultValue);

	return defaultValue;
}

int64_t Configuration::Read(const std::string &key, const int64_t defaultValue) {
	if (m_configJson.contains(key) && m_configJson[key].is_number_integer()) {
		return m_configJson[key].get<int64_t>();
	}

	Write(key, defaultValue);

	return defaultValue;
}

uint64_t Configuration::Read(const std::string &key, const uint64_t defaultValue) {
	if (m_configJson.contains(key) && m_configJson[key].is_number_unsigned()) {
		return m_configJson[key].get<uint64_t>();
	}

	Write(key, defaultValue);

	return defaultValue;
}

bool Configuration::Read(const std::string &key, const bool defaultValue) {
	if (m_configJson.contains(key) && m_configJson[key].is_boolean()) {
		return m_configJson[key].get<bool>();
	}

	Write(key, defaultValue);

	return defaultValue;
}

double Configuration::Read(const std::string &key, const double defaultValue) {
	if (m_configJson.contains(key) && m_configJson[key].is_number()) {
		return m_configJson[key].get<double>();
	}

	Write(key, defaultValue);

	return defaultValue;
}

nlohmann::json Configuration::Get(const std::string &key) const {
	return m_configJson.contains(key) ? m_configJson[key] : nlohmann::json();
}

void Configuration::Write(const std::string &key, const char *value) {
	Write(key, std::string(value));
}

void Configuration::Write(const std::string &key, const std::string &value) {
	m_configJson[key] = value;
	Save();
}

void Configuration::Write(const std::string &key, const bool value) {
	m_configJson[key] = value;
	Save();
}

void Configuration::Write(const std::string &key, const int64_t value) {
	m_configJson[key] = value;
	Save();
}

void Configuration::Write(const std::string &key, const uint64_t value) {
	m_configJson[key] = value;
	Save();
}

void Configuration::Write(const std::string &key, const double value) {
	m_configJson[key] = value;
	Save();
}
