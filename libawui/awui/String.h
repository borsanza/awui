// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUISTRING_H__
#define __AWUISTRING_H__

#include <string>
#include <awui/Object.h>

namespace awui {
	class Object;

	class String final : public Object {
		private:
			std::string value;
		public:
			static const String Empty;
			String();
			String(const char * value);
			virtual ~String();

			int GetLength();

			const char * ToCharArray() const;

			static int Compare(String strA, String strB);
			int IndexOf(String value);
			int CompareTo(String strB);

			bool operator==(String value);
			bool operator!=(String value);
			bool operator>(String value);
			bool operator<(String value);
			String operator+(String strB);
			void operator+=(String strB);
			String operator+(Object *value);
			char operator[](int pos);

			String ToUpper();
			String ToLower();

			static String Concat(String str0, String str1);
			static String Concat(String str0, String str1, String str2);
			static String Concat(String str0, String str1, String str2, String str3);

			bool Contains(String strB);

			String Substring(int startIndex);
			String Substring(int startIndex, int length);

			virtual String ToString();
	};
}

#endif
