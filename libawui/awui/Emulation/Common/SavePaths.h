#pragma once

#include <awui/String.h>

#include <string>
#include <vector>

namespace awui::Emulation::Common {
	// Dónde se guardan las partidas. Los emuladores calculan la ruta de siempre, junto a la ROM, y estas funciones la
	// llevan a la carpeta de guardado repitiendo la estructura de la de ROMs, en saves/ lo que guarda el propio juego
	// (la RAM del cartucho) y en states/ los estados del emulador (CPU, memoria... al pulsar la tecla, o al salir):
	//   roms/mastersystem/Golvellius.sav          ->  <guardado>/saves/mastersystem/Golvellius.sav
	//   roms/mastersystem/Golvellius.sms.state    ->  <guardado>/states/mastersystem/Golvellius.sms.state
	// Así se puede jugar con las ROMs en una carpeta de solo lectura. Sin configurar, todo sigue junto a la ROM.
	// Puede haber varias carpetas de ROMs (la del usuario y la que trae el programa): vale la que contenga la ROM
	class SavePaths {
	  private:
		static inline std::string s_saveDirectory;
		static inline std::vector<std::string> s_romsDirectories;

	  public:
		enum class Kind {
			Save,  // Lo que guarda el juego (RAM del cartucho, .sav): saves/
			State, // Estados del emulador (.state, .autostate): states/
		};

	  private:
		static std::string Translate(const std::string &path, Kind kind);

	  public:
		static void Configure(const String &saveDirectory, const std::vector<String> &romsDirectories);

		// Carpeta de datos del usuario para una aplicación: Environment::GetFolderPath(LocalApplicationData)/<app>,
		// es decir $XDG_DATA_HOME/<app> o ~/.local/share/<app> (en Windows, %LOCALAPPDATA%\<app>). Vacía si no se sabe
		static String GetDefaultDirectory(const char *application);

		// Para escribir: crea las carpetas que falten
		static String GetWritePath(const String &pathNextToRom, Kind kind);

		// Para leer: la nueva ruta si existe; si no, la de junto a la ROM si existe (partidas de antes)
		static String GetReadPath(const String &pathNextToRom, Kind kind);
	};
} // namespace awui::Emulation::Common
