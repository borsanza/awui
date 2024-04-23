/**
 * awui/String.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "String.h"

#include <algorithm>
#include <cstdarg>
#include <format>
#include <vector>

using namespace awui;

String::String() : m_string("") {
}

String::String(const char value) {
	m_string = std::string(1, value);
}

String::String(const char *format, ...) {
	va_list args;
	va_start(args, format);

	int n = vsnprintf(nullptr, 0, format, args);
	va_end(args);

	if (n < 0) {
		return;
	}

	std::vector<char> buffer(n + 1);
	va_start(args, format);
	vsnprintf(buffer.data(), buffer.size(), format, args);
	va_end(args);

	m_string.assign(buffer.data(), n);
}

int String::GetLength() const {
	return m_string.length();
}

bool awui::String::IsEmpty() const {
	return m_string.empty();
}

const char *String::ToCharArray() const {
	return m_string.c_str();
}

int awui::String::Compare(const String &strA, const String &strB) {
	return strA.m_string.compare(strB.m_string);
}

int String::CompareTo(const String &strB) const {
	return m_string.compare(strB.m_string);
}

bool String::EndsWith(const String &value) const {
	return (IndexOf(value, GetLength() - value.GetLength()) != -1);
}

bool String::operator==(const String &value) const {
	return (CompareTo(value) == 0);
}

bool String::operator!=(const String &value) const {
	return (CompareTo(value) != 0);
}

bool String::operator>(const String &value) const {
	return (CompareTo(value) > 0);
}

bool String::operator>=(const String &value) const {
	return (CompareTo(value) >= 0);
}

bool String::operator<(const String &value) const {
	return (CompareTo(value) < 0);
}

bool String::operator<=(const String &value) const {
	return (CompareTo(value) <= 0);
}

String String::operator+(const String &strB) const {
	return String((m_string + strB.m_string).c_str());
}

void String::operator+=(const String &strB) {
	*this = *this + strB;
}

String String::operator+(Object *value) const {
	return *this + value->ToString();
}

char String::operator[](int pos) const {
	return m_string[pos];
}

String String::ToUpper() const {
	String str2;
	str2.m_string.reserve(m_string.length());
	std::transform(m_string.begin(), m_string.end(), std::back_inserter(str2.m_string), ::toupper);

	return str2;
}

String String::ToLower() const {
	String str2;
	str2.m_string.reserve(m_string.length());
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
	return m_string.find(strB.m_string) != std::string::npos;
}

String String::Substring(int startIndex) const {
	return String(m_string.substr(startIndex).c_str());
}

String String::Substring(int startIndex, int length) const {
	return String(m_string.substr(startIndex, length).c_str());
}

String String::ToString() const {
	return String(m_string.c_str());
}

int String::IndexOf(const String &value, int startIndex) const {
	size_t pos = m_string.find(value.m_string, startIndex);
	return (pos != std::string::npos) ? pos : -1;
}

int String::LastIndexOf(const String &value) const {
	size_t pos = m_string.rfind(value.m_string);
	return (pos != std::string::npos) ? pos : -1;
}

ArrayList String::Split(const String &delimiter) const {
	ArrayList list;
	size_t startPos = 0;
	size_t endPos;

	while ((endPos = m_string.find(delimiter.m_string, startPos)) != std::string::npos) {
		String token(m_string.substr(startPos, endPos - startPos).c_str());
		list.Add(new String(token));
		startPos = endPos + delimiter.GetLength();
	}

	// Agrega el último token
	if (startPos < m_string.length()) { // Asegurarse de que hay algo que agregar
		String token(m_string.substr(startPos).c_str());
		list.Add(new String(token));
	}

	return list;
}
