// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Int32.h"

#include <awui/String.h>
#include <sstream>

using namespace awui;

Int32::Int32() {
	this->value = 0;
}

Int32::Int32(int value) {
	this->value = value;
}

Int32::~Int32() {
}

String Int32::ToString() {
	std::stringstream ss;
	ss << this->value;

	return ss.str().c_str();
}
