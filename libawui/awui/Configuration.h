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

		std::string Read(const std::string &key, const std::string &defaultValue);
		bool Read(const std::string &key, const bool defaultValue);
		int64_t Read(const std::string &key, const int64_t defaultValue);
		uint64_t Read(const std::string &key, const uint64_t defaultValue);
		double Read(const std::string &key, const double defaultValue);

		void Write(const std::string &key, const std::string &value);
		void Write(const std::string &key, const bool value);
		void Write(const std::string &key, const int64_t value);
		void Write(const std::string &key, const uint64_t value);
		void Write(const std::string &key, const double value);
	};
} // namespace awui
