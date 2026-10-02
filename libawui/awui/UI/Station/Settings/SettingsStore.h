#pragma once

#include <awui/String.h>

#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

namespace awui::UI::Station::Settings {
	// Ajustes de StationTV: el esquema (menú, tipos y valores por defecto) sale de menu-settings.json
	// y los valores elegidos se guardan en settings.json
	class SettingsStore {
	  private:
		nlohmann::json m_menu;
		nlohmann::json m_defaults; // clave -> defaultValue del esquema
		nlohmann::json m_options;  // clave de una lista -> códigos válidos

		SettingsStore();
		void CollectDefaults(const nlohmann::json &items);

	  public:
		static SettingsStore &Instance();

		// Dónde se guardan los valores (por defecto, settings.json en la carpeta actual). Antes de leer ninguno.
		// No son inline: en Windows, el ejecutable y la DLL tendrían cada uno su copia de la variable
		static void SetValuesFile(const String &path);
		static const String &GetValuesFile();

		const nlohmann::json &GetMenu() const { return m_menu; }

		// Si el valor guardado falta, es de otro tipo o no es una opción de la lista, se usa el del esquema
		// (sin guardarlo: así un valor por defecto que cambie en el esquema llega a todos)
		bool GetBool(const std::string &key);
		std::string GetString(const std::string &key);
		void SetBool(const std::string &key, bool value);
		void SetString(const std::string &key, const std::string &value);

		// Opciones de una lista del esquema: código y clave de traducción de su nombre.
		// "options": "languages" en vez de un array es la lista de idiomas disponibles (ficheros de lang/)
		static std::vector<std::pair<std::string, std::string>> GetOptions(const nlohmann::json &item);
	};
} // namespace awui::UI::Station::Settings
