#pragma once

#include <awui/String.h>

#include <nlohmann/json.hpp>
#include <string>

namespace awui::Windows::Forms::Station::Settings {
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

		const nlohmann::json &GetMenu() const { return m_menu; }

		// Si el valor guardado falta, es de otro tipo o no es una opción de la lista, se usa el del esquema
		// (sin guardarlo: así un valor por defecto que cambie en el esquema llega a todos)
		bool GetBool(const std::string &key);
		std::string GetString(const std::string &key);
		void SetBool(const std::string &key, bool value);
		void SetString(const std::string &key, const std::string &value);

		// Un texto del esquema puede ser una cadena o un objeto por idioma: {"en_US": "...", "es_ES": "..."}
		String Translate(const nlohmann::json &text);
		// Textos fijos de la interfaz del menú
		String Text(const std::string &id);
	};
} // namespace awui::Windows::Forms::Station::Settings
