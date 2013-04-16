// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "String.h"
#include <algorithm>

using namespace awui;

String::String() {
}

String::String(const char * value) {
	this->value.assign(value);
}

String::~String() {
}

int String::GetLength() {
	return this->value.length();
}

const char * String::ToCharArray() const {
	return this->value.c_str();
}

int String::Compare(String strA, String strB) {
	return strA.value.compare(strB.value);
}

int String::CompareTo(String strB) {
	return this->value.compare(strB.value);
}

bool String::operator==(String value) {
	return (String::Compare(*this, value) == 0);
}

bool String::operator!=(String value) {
	return (String::Compare(*this, value) != 0);
}

String String::operator+(String strB) {
	return String((this->value + strB.value).c_str());
}

char String::operator[](int pos) {
	return this->value[pos];
}

String String::ToUpper() {
	String str2;
	std::transform(this->value.begin(), this->value.end(), str2.value.begin(), ::toupper);

	return str2;
}

String String::ToLower() {
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

bool String::Contains(String strB) {
	int pos = this->value.find(strB.value);

	return ((pos >=0) && (pos < this->GetLength()));
}

String String::Substring(int startIndex) {
	return String(this->value.substr(startIndex).c_str());
}

String String::Substring(int startIndex, int length) {
	return String(this->value.substr(startIndex, length).c_str());
}
