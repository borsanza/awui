// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Float.h"

#include <awui/String.h>
#include <sstream>

using namespace awui;

Float::Float() {
	this->value = 0;
}

Float::Float(float value) {
	this->value = value;
}

Float::~Float() {
}

String Float::ToString() {
	std::stringstream ss;
	ss.precision(7);
	ss << this->value;

	return ss.str().c_str();
}
