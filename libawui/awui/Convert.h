#pragma once

#include <cstdint>

namespace awui {
	class String;
}

namespace awui {
	class Convert final {
	  public:
		Convert() = delete; // Solo métodos estáticos

		static String ToString(int value);
		static String ToString(float value);
		static String ToString(float value, int precision);
		static String ToString(unsigned char value);
		static String ToString(char value);

		// Texto a número, siempre con punto decimal (no depende del idioma del sistema). Se aceptan espacios al
		// principio y al final y un signo delante; cualquier otra cosa (letras, número a medias, fuera de rango,
		// infinito) no es válida. TryParse dice si lo era y solo entonces cambia value
		static bool TryParse(const String &text, int32_t &value);
		static bool TryParse(const String &text, int64_t &value);
		static bool TryParse(const String &text, float &value);
		static bool TryParse(const String &text, double &value);

		// Como TryParse, pero devuelven defaultValue si el texto no es un número válido
		static int32_t ToInt32(const String &text, int32_t defaultValue = 0);
		static int64_t ToInt64(const String &text, int64_t defaultValue = 0);
		static float ToFloat(const String &text, float defaultValue = 0.0f);
		static double ToDouble(const String &text, double defaultValue = 0.0);
	};
} // namespace awui
