/*
 * awui/Convert.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Convert.h"

#include <awui/Math.h>
#include <awui/String.h>
#include <charconv>
#include <cmath>
#include <sstream>
#include <string_view>
#include <type_traits>

using namespace awui;

String Convert::ToString(int value) {
	std::stringstream ss;
	ss << value;

	return ss.str().c_str();
}

String Convert::ToString(float value) {
	std::stringstream ss;
	ss.precision(7);
	ss << value;

	return ss.str().c_str();
}

String Convert::ToString(float value, int precision) {
	float p = Math::Pow(10.0f, precision);
	value = Math::Round(value * p) / p;

	return Convert::ToString(value);
}

String Convert::ToString(unsigned char value) {
	return ToString((int) value);
}

String Convert::ToString(char value) {
	std::stringstream ss;
	ss << value;

	return ss.str().c_str();
}

namespace {
	// Número entero o decimal ocupando todo el texto (quitando espacios y un '+' delante, que from_chars no acepta)
	template <typename T>
	bool ParseNumber(const awui::String &text, T &value) {
		std::string_view view(text.ToStdString());
		const char *spaces = " \t\n\r\f\v";
		size_t first = view.find_first_not_of(spaces);
		if (first == std::string_view::npos)
			return false;

		view = view.substr(first, view.find_last_not_of(spaces) - first + 1);
		if ((view.size() > 1) && (view[0] == '+') && (view[1] != '-'))
			view.remove_prefix(1);

		T result;
		auto [end, error] = std::from_chars(view.data(), view.data() + view.size(), result);
		if ((error != std::errc()) || (end != view.data() + view.size()))
			return false;

		// Infinito y NaN no son números útiles en ningún texto que se lea (ajustes, ficheros)
		if constexpr (std::is_floating_point_v<T>) {
			if (!std::isfinite(result))
				return false;
		}

		value = result;
		return true;
	}

	template <typename T>
	T ParseOrDefault(const awui::String &text, T defaultValue) {
		T value;
		return ParseNumber(text, value) ? value : defaultValue;
	}
} // namespace

bool Convert::TryParse(const String &text, int32_t &value) {
	return ParseNumber(text, value);
}

bool Convert::TryParse(const String &text, int64_t &value) {
	return ParseNumber(text, value);
}

bool Convert::TryParse(const String &text, float &value) {
	return ParseNumber(text, value);
}

bool Convert::TryParse(const String &text, double &value) {
	return ParseNumber(text, value);
}

int32_t Convert::ToInt32(const String &text, int32_t defaultValue) {
	return ParseOrDefault(text, defaultValue);
}

int64_t Convert::ToInt64(const String &text, int64_t defaultValue) {
	return ParseOrDefault(text, defaultValue);
}

float Convert::ToFloat(const String &text, float defaultValue) {
	return ParseOrDefault(text, defaultValue);
}

double Convert::ToDouble(const String &text, double defaultValue) {
	return ParseOrDefault(text, defaultValue);
}
