/**
 * awui/String.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "String.h"

#include <algorithm>
#include <awui/Console.h>
#include <awui/Object.h>

using namespace awui;

String::String() {
}

String::String(const char * value) {
	this->value.assign(value);
}

String::~String() {
}

int String::GetLength() const {
	return this->value.length();
}

const char * String::ToCharArray() const {
	return this->value.c_str();
}

int String::Compare(String strA, String strB) {
	return strA.value.compare(strB.value);
}

int String::CompareTo(String strB) const {
	return this->value.compare(strB.value);
}

bool String::EndsWith(String value) const {
	return (IndexOf(value, this->GetLength() - value.GetLength()) != -1);
}

bool String::operator==(String value) {
	return (String::Compare(*this, value) == 0);
}

bool String::operator!=(String value) {
	return (String::Compare(*this, value) != 0);
}

bool String::operator>(String value) {
	return (String::Compare(*this, value) > 0);
}

bool String::operator<(String value) {
	return (String::Compare(*this, value) < 0);
}

String String::operator+(String strB) {
	return String((this->value + strB.value).c_str());
}

void String::operator+=(String strB) {
	*this = *this + strB;
}

String String::operator+(Object *value) {
	String a = *this + value->ToString();
	return a;
}

char String::operator[](int pos) {
	return this->value[pos];
}

String String::ToUpper() const {
	String str2;
	std::transform(this->value.begin(), this->value.end(), str2.value.begin(), ::toupper);

	return str2;
}

String String::ToLower() const {
	String str2;
	std::transform(this->value.begin(), this->value.end(), str2.value.begin(), ::tolower);

	return str2;
}

String String::Concat(String str0, String str1) {
	return (str0 + str1);
}

String String::Concat(String str0, String str1, String str2) {
	return ((str0 + str1) + str2);
}

String String::Concat(String str0, String str1, String str2, String str3) {
	return ((str0 + str1) + (str2 + str3));
}

bool String::Contains(String strB) const {
	int pos = this->value.find(strB.value);

	return ((pos >=0) && (pos < this->GetLength()));
}

String String::Substring(int startIndex) const {
	return String(this->value.substr(startIndex).c_str());
}

String String::Substring(int startIndex, int length) const {
	return String(this->value.substr(startIndex, length).c_str());
}

String String::ToString() {
	return String(this->value.c_str());
}

int String::IndexOf(String value, int startIndex) const {
	std::string::size_type loc = this->value.find(value.value, startIndex);
	if (loc != std::string::npos)
		return loc;

	return -1;
}

int String::LastIndexOf(String value) const {
	std::string::size_type loc = this->value.find_last_of(value.value);
	if (loc != std::string::npos)
		return loc;

	return -1;
}

ArrayList String::Split(const String value) const {
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
