#pragma once

#include <awui/String.h>

#include <string>

namespace awui::Emulation::Common {
	// Dónde se guardan las partidas (.sav, estados). Los emuladores calculan la ruta de siempre, junto a la ROM,
	// y estas funciones la llevan a la carpeta de guardado repitiendo la estructura de la de ROMs:
	//   roms/mastersystem/Golvellius.sav  ->  <guardado>/mastersystem/Golvellius.sav
	// Así se puede jugar con las ROMs en una carpeta de solo lectura. Sin configurar, todo sigue junto a la ROM.
	class SavePaths {
	  private:
		static inline std::string m_saveDirectory;
		static inline std::string m_romsDirectory;

		static std::string Translate(const std::string &path);

	  public:
		static void Configure(const String &saveDirectory, const String &romsDirectory);

		// Carpeta de datos del usuario para una aplicación: $XDG_DATA_HOME/<app> o ~/.local/share/<app>
		// (en Windows, %APPDATA%\<app>)
		static String GetDefaultDirectory(const char *application);

		// Para escribir: crea las carpetas que falten
		static String GetWritePath(const String &pathNextToRom);

		// Para leer: la nueva ruta si existe; si no, la de junto a la ROM si existe (partidas de antes)
		static String GetReadPath(const String &pathNextToRom);
	};
} // namespace awui::Emulation::Common
