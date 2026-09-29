#pragma once

#include <awui/String.h>

#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

namespace awui {
	// Traducciones: un fichero por idioma (lang/<código>.json) con pares "clave": "texto".
	// Cada fichero incluye "language.name" con el nombre del idioma tal como se muestra en el menú.
	class Localization {
	  private:
		static inline std::string m_directory = "lang";
		static inline std::string m_language;
		static inline nlohmann::json m_texts = nlohmann::json::object();
		static inline nlohmann::json m_fallback = nlohmann::json::object();

		static nlohmann::json LoadFile(const std::string &code);

	  public:
		static const char *FallbackLanguage;

		static void SetDirectory(const std::string &directory);
		static void SetLanguage(const std::string &code);
		static inline const std::string &GetLanguage() { return m_language; }

		// Texto de la clave en el idioma elegido; si falta, en inglés; si tampoco, la propia clave
		// (así un texto sin traducir se ve enseguida, y un texto que no es clave, como "25%", sale tal cual)
		static String Tr(const std::string &key);

		// Idiomas disponibles (los ficheros de lang/), ordenados por código: código y nombre
		static std::vector<std::pair<std::string, String>> GetLanguages();
	};
} // namespace awui
