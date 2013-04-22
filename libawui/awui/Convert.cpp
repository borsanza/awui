// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Convert.h"

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

String Convert::ToString(unsigned char value) {
	return ToString((int) value);
}
