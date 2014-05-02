// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Convert.h"

#include <awui/Math.h>
#include <awui/String.h>
#include <sstream>

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
