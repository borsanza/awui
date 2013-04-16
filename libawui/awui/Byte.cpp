// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Byte.h"

#include <awui/String.h>
#include <sstream>

using namespace awui;

Byte::Byte() {
	this->value = 0;
}

Byte::Byte(unsigned char value) {
	this->value = value;
}

Byte::~Byte() {
}

String Byte::ToString() {
	std::stringstream ss;
	ss << (int)this->value;

	return ss.str().c_str();
}
