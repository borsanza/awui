// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Object.h"

using namespace awui;

Object::Object() {
}

Object::~Object() {
}

int Object::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Object)
		return 1;

	return 0;
}
