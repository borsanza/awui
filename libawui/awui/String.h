#pragma once

#include <awui/Collections/ArrayList.h>
#include <awui/Object.h>

using namespace awui::Collections;

namespace awui {
	class String final : public Object {
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

		virtual ~String() = default;

		int GetLength() const;

		bool IsEmpty() const;

		const char *ToCharArray() const;

		static int Compare(const String &strA, const String &strB);
		int IndexOf(const String &value, int startIndex = 0) const;
		int LastIndexOf(const String &value) const;
		int CompareTo(const String &strB) const;
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

		String ToUpper() const;
		String ToLower() const;

		static String Concat(const String &str0, const String &str1);
		static String Concat(const String &str0, const String &str1, const String &str2);
		static String Concat(const String &str0, const String &str1, const String &str2, const String &str3);

		bool Contains(const String &strB) const;

		String Substring(int startIndex) const;
		String Substring(int startIndex, int length) const;

		ArrayList Split(const String &value) const;

		virtual String ToString() const override;
	};
} // namespace awui
