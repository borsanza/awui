/**
 * awui/String.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "String.h"

#include <algorithm>

using namespace awui;

String::String() {
}

String::String(const char * value) {
	m_string.assign(value);
}

String::String(const char value) {
	m_string = std::string(1, value);
}

String::~String() {
}

int String::GetLength() const {
	return this->m_string.length();
}

const char * String::ToCharArray() const {
	return m_string.c_str();
}

int awui::String::Compare(const String &strA, const String &strB) {
	return strA.m_string.compare(strB.m_string);
}

int String::CompareTo(const String &strB) const {
	return m_string.compare(strB.m_string);
}

bool String::EndsWith(const String &value) const {
	return (IndexOf(value, this->GetLength() - value.GetLength()) != -1);
}

bool String::operator==(const String &value) const {
	return (this->CompareTo(value) == 0);
}

bool String::operator!=(const String &value) const {
	return (this->CompareTo(value) != 0);
}

bool String::operator>(const String &value) const {
	return (this->CompareTo(value) > 0);
}

bool String::operator>=(const String &value) const {
	return (this->CompareTo(value) >= 0);
}

bool String::operator<(const String &value) const {
	return (this->CompareTo(value) < 0);
}

bool String::operator<=(const String &value) const {
	return (this->CompareTo(value) <= 0);
}

String String::operator+(const String &strB) const {
	return String((m_string + strB.m_string).c_str());
}

void String::operator+=(const String &strB) {
	*this = *this + strB;
}

String String::operator+(Object *value) const {
	String a = *this + value->ToString();
	return a;
}

char String::operator[](int pos) const {
	return m_string[pos];
}

String String::ToUpper() const {
	String str2;
	std::transform(m_string.begin(), m_string.end(), std::back_inserter(str2.m_string), ::toupper);
	
	return str2;
}

String String::ToLower() const {
	String str2;
	std::transform(m_string.begin(), m_string.end(), std::back_inserter(str2.m_string), ::tolower);

	return str2;
}

String String::Concat(const String &str0, const String &str1) {
	return (str0 + str1);
}

String String::Concat(const String &str0, const String &str1, const String &str2) {
	return ((str0 + str1) + str2);
}

String String::Concat(const String &str0, const String &str1, const String &str2, const String &str3) {
	return ((str0 + str1) + (str2 + str3));
}

bool String::Contains(const String &strB) const {
	int pos = m_string.find(strB.m_string);

	return ((pos >=0) && (pos < this->GetLength()));
}

String String::Substring(int startIndex) const {
	return String(m_string.substr(startIndex).c_str());
}

String String::Substring(int startIndex, int length) const {
	return String(m_string.substr(startIndex, length).c_str());
}

String String::ToString() {
	return String(m_string.c_str());
}

int String::IndexOf(const String &value, int startIndex) const
{
    std::string::size_type loc = m_string.find(value.m_string, startIndex);
	if (loc != std::string::npos)
		return loc;

	return -1;
}

int String::LastIndexOf(const String &value) const {
	std::string::size_type loc = m_string.find_last_of(value.m_string);
	if (loc != std::string::npos)
		return loc;

	return -1;
}

ArrayList String::Split(const String &value) const {
	ArrayList list;

	int pre = 0;
	int pos = 0;
	do {
		String pru;
		pos = this->IndexOf(value, pos);
		if (pos == -1) {
			pru = this->Substring(pre);
		} else {
			pru = this->Substring(pre, pos - pre);
		}

		pre = pos + 1;

		if (pos != -1)
			pos++;

		list.Add(new String(pru));
	} while (pos != -1);

	return list;
}
