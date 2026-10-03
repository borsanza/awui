#pragma once

#include <awui/Object.h>

#include <functional>
#include <string>
#include <vector>

namespace awui {
	// Tipo de valor: sin herencia ni métodos virtuales (envuelve un std::string)
	class String final {
	  private:
		std::string m_string;

		void AssignFormat(const char *format, ...);

	  public:
		String();
		String(const char);
		// Copia el texto tal cual: un '%' (por ejemplo en un nombre de fichero) no se interpreta como formato
		String(const char *value);
		// Formato de printf. Solo se usa cuando hay al menos un argumento además del formato
		template <typename Arg, typename... Args>
		String(const char *format, Arg arg, Args... args) {
			AssignFormat(format, arg, args...);
		}
		String(const std::string &value) : m_string(value) {}
		String(std::string &&value) : m_string(std::move(value)) {}

		// El std::string de dentro, para usar la biblioteca estándar sin copiar
		inline const std::string &ToStdString() const { return m_string; }

		int GetLength() const;

		bool IsEmpty() const;

		const char *ToCharArray() const;

		static int Compare(const String &strA, const String &strB);
		int IndexOf(const String &value, int startIndex = 0) const;
		int IndexOf(char value, int startIndex = 0) const;
		int LastIndexOf(const String &value) const;
		int LastIndexOf(char value) const;
		int CompareTo(const String &strB) const;
		bool StartsWith(const String &value) const;
		bool EndsWith(const String &value) const;

		bool operator==(const String &value) const;
		bool operator!=(const String &value) const;
		bool operator>(const String &value) const;
		bool operator>=(const String &value) const;
		bool operator<(const String &value) const;
		bool operator<=(const String &value) const;
		String operator+(const String &strB) const;
		void operator+=(const String &strB);
		String operator+(Object *value) const;
		char operator[](int pos) const;

		// Solo cambian las letras ASCII: los bytes de UTF-8 (acentos, ñ, cirílico...) se dejan como están
		String ToUpper() const;
		String ToLower() const;

		// Sin espacios, tabuladores ni saltos de línea al principio y/o al final
		String Trim() const;
		String TrimStart() const;
		String TrimEnd() const;

		// Todas las apariciones de oldValue (vacío: no cambia nada)
		String Replace(const String &oldValue, const String &newValue) const;

		static String Concat(const String &str0, const String &str1);
		static String Concat(const String &str0, const String &str1, const String &str2);
		static String Concat(const String &str0, const String &str1, const String &str2, const String &str3);

		bool Contains(const String &strB) const;

		// Fuera de rango se recorta (no lanza excepciones): un inicio más allá del final da una cadena vacía
		String Substring(int startIndex) const;
		String Substring(int startIndex, int length) const;

		// Los trozos entre delimitadores, también los vacíos (como en .NET): "a,b," da [a, b, ""], ",a" da ["", a] y
		// "" da [""]. Con un delimitador vacío, la cadena entera
		std::vector<String> Split(const String &delimiter) const;

		String ToString() const;
	};
} // namespace awui

// Para usar String como clave en std::unordered_map / std::unordered_set
template <>
struct std::hash<awui::String> {
	size_t operator()(const awui::String &value) const noexcept { return std::hash<std::string>()(value.ToStdString()); }
};
