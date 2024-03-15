#pragma once

#include <awui/Collections/ArrayList.h>
#include <awui/Object.h>

using namespace awui::Collections;

namespace awui {
	class String final : public Object {
	  private:
		std::string m_string;

	  public:
		String();
		String(const char);
		String(const char *value);
		virtual ~String() = default;

		int GetLength() const;

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
