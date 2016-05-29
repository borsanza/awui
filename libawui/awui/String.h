#pragma once

#include <string>
#include <awui/Object.h>
#include <awui/Collections/ArrayList.h>

using namespace awui::Collections;

namespace awui {
	class String final : public Object {
		private:
			std::string value;
		public:
			String();
			String(const char * value);
			virtual ~String();

			int GetLength() const;

			const char * ToCharArray() const;

			static int Compare(String strA, String strB);
			int IndexOf(String value, int startIndex = 0) const;
			int LastIndexOf(String value) const;
			int CompareTo(String strB) const;
			bool EndsWith(String value) const;

			bool operator==(String value);
			bool operator!=(String value);
			bool operator>(String value);
			bool operator<(String value);
			String operator+(String strB);
			void operator+=(String strB);
			String operator+(Object *value);
			char operator[](int pos);

			String ToUpper() const;
			String ToLower() const;

			static String Concat(String str0, String str1);
			static String Concat(String str0, String str1, String str2);
			static String Concat(String str0, String str1, String str2, String str3);

			bool Contains(String strB) const;

			String Substring(int startIndex) const;
			String Substring(int startIndex, int length) const;

			ArrayList Split(const String value) const;

			virtual String ToString();
	};
}
