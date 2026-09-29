#pragma once

#include <nlohmann/json.hpp>

#include <awui/String.h>
#include <string>

namespace awui {
	class String;

	class Configuration {
	  private:
		nlohmann::json m_configJson;
		String m_currentFilePath;

		Configuration(const String &filePath);

		bool Load(const String &filePath);

		void Save();

		// Prevent copying and assignment
		Configuration(const Configuration &) = delete;
		Configuration &operator=(const Configuration &) = delete;

	  public:
		static Configuration &getInstance(const String &filePath);

		// Las versiones con const char * evitan que un literal acabe en la de bool
		std::string Read(const std::string &key, const char *defaultValue);
		std::string Read(const std::string &key, const std::string &defaultValue);
		bool Read(const std::string &key, const bool defaultValue);
		int64_t Read(const std::string &key, const int64_t defaultValue);
		uint64_t Read(const std::string &key, const uint64_t defaultValue);
		double Read(const std::string &key, const double defaultValue);

		// Valor guardado tal cual (null si no existe), sin escribir nada
		nlohmann::json Get(const std::string &key) const;

		void Write(const std::string &key, const char *value);
		void Write(const std::string &key, const std::string &value);
		void Write(const std::string &key, const bool value);
		void Write(const std::string &key, const int64_t value);
		void Write(const std::string &key, const uint64_t value);
		void Write(const std::string &key, const double value);
	};
} // namespace awui
