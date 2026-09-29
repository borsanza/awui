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

String::String(const char *value) : m_string(value ? value : "") {
}

void String::AssignFormat(const char *format, ...) {
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

bool String::StartsWith(const String &value) const {
	return m_string.compare(0, value.m_string.length(), value.m_string) == 0;
}

bool String::EndsWith(const String &value) const {
	size_t length = value.m_string.length();
	return (length <= m_string.length()) && (m_string.compare(m_string.length() - length, length, value.m_string) == 0);
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
	return String(m_string + strB.m_string);
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
	// toupper con un char negativo (bytes de UTF-8) es comportamiento indefinido: se pasa como unsigned char
	std::transform(m_string.begin(), m_string.end(), std::back_inserter(str2.m_string), [](unsigned char c) { return (char) ::toupper(c); });

	return str2;
}

String String::ToLower() const {
	String str2;
	str2.m_string.reserve(m_string.length());
	std::transform(m_string.begin(), m_string.end(), std::back_inserter(str2.m_string), [](unsigned char c) { return (char) ::tolower(c); });

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
	if ((startIndex < 0) || ((size_t) startIndex >= m_string.length()))
		return String((startIndex < 0) ? m_string : std::string());

	return String(m_string.substr(startIndex));
}

String String::Substring(int startIndex, int length) const {
	if (startIndex < 0) {
		length += startIndex;
		startIndex = 0;
	}

	if ((length <= 0) || ((size_t) startIndex >= m_string.length()))
		return String();

	return String(m_string.substr(startIndex, length));
}

static bool IsSpace(char c) {
	return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == '\v') || (c == '\f');
}

String String::TrimStart() const {
	size_t start = 0;
	while ((start < m_string.length()) && IsSpace(m_string[start]))
		start++;

	return String(m_string.substr(start));
}

String String::TrimEnd() const {
	size_t end = m_string.length();
	while ((end > 0) && IsSpace(m_string[end - 1]))
		end--;

	return String(m_string.substr(0, end));
}

String String::Trim() const {
	return TrimStart().TrimEnd();
}

String String::Replace(const String &oldValue, const String &newValue) const {
	if (oldValue.m_string.empty())
		return *this;

	std::string result;
	size_t pos = 0;
	size_t found;
	while ((found = m_string.find(oldValue.m_string, pos)) != std::string::npos) {
		result.append(m_string, pos, found - pos);
		result.append(newValue.m_string);
		pos = found + oldValue.m_string.length();
	}

	result.append(m_string, pos, std::string::npos);
	return String(std::move(result));
}

String String::ToString() const {
	return String(m_string.c_str());
}

int String::IndexOf(const String &value, int startIndex) const {
	size_t pos = m_string.find(value.m_string, startIndex);
	return (pos != std::string::npos) ? pos : -1;
}

int String::IndexOf(char value, int startIndex) const {
	if (startIndex < 0)
		startIndex = 0;

	size_t pos = m_string.find(value, startIndex);
	return (pos != std::string::npos) ? (int) pos : -1;
}

int String::LastIndexOf(char value) const {
	size_t pos = m_string.rfind(value);
	return (pos != std::string::npos) ? (int) pos : -1;
}

int String::LastIndexOf(const String &value) const {
	size_t pos = m_string.rfind(value.m_string);
	return (pos != std::string::npos) ? pos : -1;
}

std::vector<awui::String> String::Split(const String &delimiter) const {
	std::vector<String> list;
	size_t startPos = 0;
	size_t endPos;

	while ((endPos = m_string.find(delimiter.m_string, startPos)) != std::string::npos) {
		String token(m_string.substr(startPos, endPos - startPos).c_str());
		list.push_back(token);
		startPos = endPos + delimiter.GetLength();
	}

	// Agrega el último token
	if (startPos < m_string.length()) { // Asegurarse de que hay algo que agregar
		String token(m_string.substr(startPos).c_str());
		list.push_back(token);
	}

	return list;
}
