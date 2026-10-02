#pragma once

#include <awui/String.h>

namespace awui {
	class Environment {
	  public:
		enum class SpecialFolder {
			ApplicationData,	  // Configuración: $XDG_CONFIG_HOME o ~/.config (en Windows, %APPDATA%)
			LocalApplicationData, // Datos: $XDG_DATA_HOME o ~/.local/share (en Windows, %LOCALAPPDATA%)
		};

		static String GetNewLine();

		static String GetFolderPath(SpecialFolder folder);
	};
} // namespace awui
